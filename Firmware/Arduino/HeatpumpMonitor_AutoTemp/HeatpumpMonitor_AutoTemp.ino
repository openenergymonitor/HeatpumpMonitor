// Heatpump monitor example with:
// MBUS meter reader for kampstrup multical 402
// Grundfos vortex flow sensor on analog 0 and 1
// 2x DS18B20 flow + return sensors
// All sending via RFM12/69.
// Licence: GPLv3
#include <avr/wdt.h>

#define FirmwareVersion = 2.0
#define DEBUG 0
#define RFM69_ENABLE 0

#define OEM_EMON_ENABLE 0
#define OEM_EMON_ACAC 1
#define DS18B20_ENABLE 1
#define KAMSTRUP_ENABLE 1
#define KAMSTRUP_MODEL 403 // 402, 403
#define VFS_ENABLE 0
#define ELSTER_IRDA_ENABLE 1
#define MBUS_ENABLE 1

// EmonTH packet
typedef struct {                                                      // RFM12B RF payload datastructure
  int temp;
  int temp_external;
  int humidity;
  int battery;
  unsigned long pulsecount;
} PayloadTH;
PayloadTH emonth;
// ------------------------------------------------------------------------------------------

#include "elster.h"
#include "EmonLib.h"                   // Include Emon Library:  https://github.com/openenergymonitor/EmonLib
EnergyMonitor ct1;                     // Create an instance
EnergyMonitor ct2;                     // Create an instance

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 19                                                  // Data wire is plugged into port 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);                                           // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);                                     // Pass our oneWire reference to Dallas Temperature.
DeviceAddress tmp_address;

// Four temperature sensors
int uids[6]; double temps[6];

// --------------------------------------------------
// Grundfos VFS config
// --------------------------------------------------
double VFS_maxflow          = 40.0;  // Litres/minute
double VFS_maxflow_voltage  = 3.5;   // Volts
double VFS_zeroflow_voltage = 0.25;  // Volts
double VFS_maxtemp          = 100;   // Celcius
double VFS_maxtemp_voltage  = 3.5;   // Volts
double VFS_zerotemp_voltage = 0.5;   // Volts
// --------------------------------------------------
// Kamstrup MBUS config
// --------------------------------------------------

byte kamstrup_mbus_address = 0;                   // Set to 0 for auto scan, (test: check timeout 10ms)
int kamstrup_failures = 0;
#include <CustomSoftwareSerial.h>
CustomSoftwareSerial* customSerial;               // Declare serial

unsigned long last = 0;
unsigned long now = 0;
unsigned long lastwdtreset = 0;

int bid = 0;
byte bytes[100];
byte dlen = 0;

#define RF_freq RF12_433MHZ                                             // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 1;                                                   // emonTx RFM12B node ID
const int networkGroup = 210;                                           // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLCD needs to be same as emonBase and emonGLCD

#define RF69_COMPAT 1 // set to 1 to use RFM69CW
#include <JeeLib.h>   // make sure V12 (latest) is used if using RFM69CW
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// CT Sensors
int OEMct1 = 0;
int OEMct2 = 0;
int joules_CT1 = 0;
int joules_CT2 = 0;
unsigned long CT1_Wh = 0;
unsigned long CT2_Wh = 0;

// Kamstrup variables
long KSkWh = 0;
int volume = 0;
int ontime = 0;
int KSflowT = 0;
int KSreturnT = 0;
int KSdeltaT = 0;
int KSheat = 0;
int maxpower = 0;
int KSflowrate = 0;
int maxflow = 0;

// VFS Variables
int VFSflowT = 0;
int VFSflowrate = 0;
int VFSheat = 0;

// pulseCounting
long pulseCount = 0;
//unsigned long pulseTime,lastTime; // Used to measure time between pulses
//double power;
//int ppwh = 1;                     // pulses per watt hour - found or set on the meter.

bool firstrun = true;
unsigned long last_reading = 0;

double VFS_Flow_Cal = 0;
double VFS_Temp_Cal = 0;

unsigned long msgnum = 0;
// -------------------------------------------------------------------
// Elster meter reading
// -------------------------------------------------------------------
void meter_reading(unsigned long r)
{
  pulseCount = r;
}

ElsterA100C meter(meter_reading);

// -------------------------------------------------------------------
// onPulse
// The interrupt routine - runs each time a falling edge of a pulse is detected
// -------------------------------------------------------------------
//
void onPulse()
{
  // lastTime = pulseTime;
  // pulseTime = micros();
  pulseCount++;                                               // count pulse
  // power = int((3600000000.0 / (pulseTime - lastTime))/ppwh);  // calculate power
}

// -------------------------------------------------------------------
// Phase Decode
// -------------------------------------------------------------------
void parse()
{
  if (KAMSTRUP_MODEL==402)
  {
    byte i=25;
    KSkWh = decodeval(i); i+=6;
    volume = decodeval(i); i+=6;
    ontime = decodeval(i); i+=6;
    KSflowT = decodeval(i); i+=6;
    KSreturnT = decodeval(i); i+=6;
    KSdeltaT = decodeval(i); i+=6;
    KSheat = decodeval(i)*100; i+=6;
    maxpower = decodeval(i)*100; i+=6;
    KSflowrate = decodeval(i); i+=6;
    maxflow = decodeval(i); i+=6;
  }
  
  if (KAMSTRUP_MODEL==403)
  {
    // Locations set here correspond to location as described in datasheet minus 9 bytes
    // 
    int offset = 9;
    KSkWh = decode_4byte_bin(21);
    // print "Cooling E3:"+str(decode_4byte_bin(30-offset))
    // print "Energy E8:"+str(decode_4byte_bin(37-offset))
    // print "Energy E9:"+str(decode_4byte_bin(44-offset))
    int volume = decode_4byte_bin(50-9);
    // print "Pulse A:"+str(decode_4byte_bin(57-offset))
    // print "Pulse B:"+str(decode_4byte_bin(65-offset))
    int ontime = decode_4byte_bin(71-offset);
    KSflowT = decode_2byte_bin(83-offset);
    KSreturnT = decode_2byte_bin(87-offset);
    KSdeltaT = decode_2byte_bin(91-offset);
    KSheat = decode_4byte_bin(95-offset);
    KSflowrate = decode_4byte_bin(107-offset);
  }
}

// -------------------------------------------------------------------
// Decode
// -------------------------------------------------------------------
long decodeval(long i) {
  return bytes[i+2] + (bytes[i+3]<<8) + (bytes[i+4]<<16) + (bytes[i+5]<<24);
}

long decode_2byte_bin(long i) {
  return bytes[i+0] + (bytes[i+1]<<8);
}

long decode_4byte_bin(long i) {
  return bytes[i+0] + (bytes[i+1]<<8) + (bytes[i+2]<<16) + (bytes[i+3]<<24);
}


// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {
  wdt_enable(WDTO_8S);
  
  Serial.begin(115200);
  if (DEBUG) Serial.println("Startup");
  if (RFM69_ENABLE) rf12_initialize(nodeID, RF_freq, networkGroup);
  sensors.begin();

  ct1.voltage(0, 262.0, 1.7);
  ct1.current(1, 20.0);
  ct2.voltage(0, 262.0, 1.7);
  ct2.current(2, 20.0);
  
  delay(100);
  
  // MBUS
  customSerial = new CustomSoftwareSerial(4, 5); // rx, tx
  customSerial->begin(2400, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1

  wdt_reset();

  if (KAMSTRUP_ENABLE)
  {
    mbus_normalize();
    if (kamstrup_mbus_address==0) {
      if (DEBUG) Serial.println("Scanning MBUS ");
      kamstrup_mbus_address = mbus_scan();
      if (kamstrup_mbus_address) {
        if (DEBUG) Serial.print("Meter found, address: ");
        if (DEBUG) Serial.println(kamstrup_mbus_address);
      } else {
        if (DEBUG) Serial.println("No MBUS meter found");
      }
    }
  }
  wdt_reset();
  if (DEBUG) Serial.println("Attached Interrupt");
  
  delay(100);
  if (!ELSTER_IRDA_ENABLE) {
    attachInterrupt(1, onPulse, FALLING);
  } else {
    meter.init(1);
  }

  CT1_Wh = 0;
  CT2_Wh = 0;
  wdt_reset();

  VFS_Flow_Cal = ((VFS_maxflow_voltage-VFS_zeroflow_voltage)/VFS_maxflow);
  VFS_Temp_Cal = ((VFS_maxtemp_voltage-VFS_zerotemp_voltage)/VFS_maxtemp);
}

// -------------------------------------------------------------------
// LOOP
// -------------------------------------------------------------------
void loop() {
  now = millis();

  if (ELSTER_IRDA_ENABLE) {
    // Decode the meter stream
    const int byte_data = meter.decode_bit_stream();
    if (byte_data != -1) {
      meter.on_data(byte_data);
    }
  }

  if (RFM69_ENABLE && rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0)
  {
    int node_id = (rf12_hdr & 0x1F);
    byte n = rf12_len;
    if (DEBUG) {
      Serial.print("RFM node:");
      Serial.print(node_id);
      Serial.print(" len:");
      Serial.println(n);
    }

    // If EmonTH on node 23 then decode names fully
    if (node_id == 23 && n == 12)
    {
      emonth = *(PayloadTH*) rf12_data;
      Serial.print("THtemp:"); Serial.print(emonth.temp*0.1,1);
      Serial.print(",THexttemp:"); Serial.print(emonth.temp_external*0.1,1);
      Serial.print(",THhumidity:"); Serial.print(emonth.humidity*0.1,1);
      Serial.print(",THbattery:"); Serial.print(emonth.battery*0.1,1);
      Serial.print(",THpulse:"); Serial.println(emonth.pulsecount);
    } else {
      // For all other received nodes send as decoded integers: 10_1:1560,10_2:200...
      for (byte i=0; i<n; i+=2)
      {
        int num = ((unsigned char)rf12_data[i+1] << 8 | (unsigned char)rf12_data[i]);
        if (i) Serial.print(",");
        Serial.print(node_id);
        Serial.print("_");
        Serial.print(i);
        Serial.print(":");
        Serial.print(num);
      }
      Serial.println();
    }
  }
  
  if ((now-last)>=9800 || firstrun) {
    wdt_reset();
    
    last = now; firstrun = false;
    bool kamstrup_reply_received = false;
    
    if (KAMSTRUP_ENABLE)
    {
      // 1) KAMSTRUP HEAT METER REQUEST:
      if (MBUS_ENABLE) {
        if (DEBUG) Serial.println("mbus_request_data");
        if (kamstrup_mbus_address>0) {
          mbus_request_data(kamstrup_mbus_address);
        } else {
          if (kamstrup_failures>10) {
            if (DEBUG) Serial.println("Mbus scan start");
            kamstrup_mbus_address = mbus_scan();
            kamstrup_failures = 0;
            wdt_reset();
          }
        }
        bid = 0;
    
        int kamstrup_timeout = 1000;
        unsigned long timer_start = millis();
        while (!kamstrup_reply_received && (millis()-timer_start)<kamstrup_timeout)
        {
          if (customSerial->available())
          {
            
            byte val = (byte) customSerial->read();
        
            bytes[bid] = val;
            bid++;
            
            if (bytes[0]!=104) {
              bid = 0;
            }
            
            if (bid==3) {
              if (bytes[1]==bytes[2]) {
                dlen = bytes[1];
              } else {
                bid = 0;
              }
            }
            
            if (bid==120)
            {
              kamstrup_reply_received = true;
              bid = 0;
            }
          }
        }
    
        if (kamstrup_reply_received==false) {
          kamstrup_failures++;
        }
      }
    }

    wdt_reset();

    delay(200);

    // -----------------------------------------------------
    // DS18B20 Temperature sensors
    // -----------------------------------------------------
    int numberOfDevices = 0;
    if (DS18B20_ENABLE) {
      // DS18B20 temp sensors
      sensors.begin();
      numberOfDevices = sensors.getDeviceCount();
      sensors.requestTemperatures();
  
      for(int i=0;i<numberOfDevices; i++)
      {
        sensors.getAddress(tmp_address, i);
        double temp = sensors.getTempC(tmp_address);
        
        unsigned long uid = 0;
        for (uint8_t i = 0; i < 8; i++) {
           // Serial.print(deviceAddress[i], HEX);
           uid += tmp_address[i];
        }
        uids[i] = uid;
        temps[i] = temp;
        
        wdt_reset();
      }
    }

    wdt_reset();
    
    // -----------------------------------------------------
    // Analog read for grundfos vortex flow sensor
    // -----------------------------------------------------
    if (VFS_ENABLE) {
      delay(200);
      unsigned long sumA3 = 0;
      unsigned long sumA4 = 0;
      for (int i=0; i<100; i++) {
        sumA3 += analogRead(3);
        sumA4 += analogRead(4);
      }
      int A3 = (int)(sumA3 / 100);
      int A4 = (int)(sumA4 / 100);
      double A3_voltage = 3.3*(A3/1023.0);
      double A4_voltage = 3.3*(A4/1023.0);
      
      double VFStemp = (A3_voltage - VFS_zerotemp_voltage) / VFS_Temp_Cal;
      if (VFStemp<0) VFStemp = 0;
  
      double VFSflow = (A4_voltage - VFS_zeroflow_voltage) / VFS_Flow_Cal;
      if (VFSflow<0) VFSflow = 0;
      if ((1*A4_voltage)<0.4) VFSflow = 0;  // Minimum voltage to accept flow reading, datasheet recommends 0.5V
      
      // double deltaT = (1.0*emontx.DSflowT-emontx.DSreturnT)*0.01;
      // double VFSheat = ((VFSflow/60.0)*4181.0) * deltaT;
    }
    
    wdt_reset();

    // -----------------------------------------------------
    // CT Sensors
    // -----------------------------------------------------
    if (OEM_EMON_ENABLE)
    {
        // Reading of CT sensors needs to go here for stability
        // need to double check the reason for this.
        for (int i=0; i<10; i++) {
          analogRead(0); analogRead(1); analogRead(2);
        }
        delay(200);
        
        if (OEM_EMON_ACAC) {
          ct1.calcVI(30,2000);
          OEMct1 = ct1.realPower;
          ct2.calcVI(30,2000);
          OEMct2 = ct2.realPower;
        } else {
          OEMct1 = 230 * ct1.calcIrms(1480);
          OEMct2 = 230 * ct2.calcIrms(1480);
        }
    
        // Accumulating Watt hours
        int interval = millis() - last_reading;
        last_reading = millis();
    
        if (ct1.realPower > 0 && interval>0) {
          unsigned long jouleinc = ct1.realPower * interval *0.001;
          joules_CT1 += jouleinc;
          CT1_Wh += joules_CT1 / 3600;
          joules_CT1 = joules_CT1 % 3600;
        }
    
        if (ct2.realPower > 0 && interval>0) {
          unsigned long jouleinc = ct2.realPower * interval *0.001;
          joules_CT2 += jouleinc;
          CT2_Wh += joules_CT2 / 3600;
          joules_CT2 = joules_CT2 % 3600;
        }
        wdt_reset();
    }

    msgnum++;
    Serial.print("Msg:"); Serial.print(msgnum);
    if (OEM_EMON_ENABLE)
    {
        Serial.print(",OEMct1:"); Serial.print(OEMct1);
        Serial.print(",OEMct2:"); Serial.print(OEMct2);
        Serial.print(",OEMct1Wh:"); Serial.print(CT1_Wh);
        Serial.print(",OEMct2Wh:"); Serial.print(CT2_Wh);
    }
    
    if (VFS_ENABLE) {
      Serial.print(",VFSflowT:"); Serial.print(VFSflowT*0.01);
      Serial.print(",VFSflowrate:"); Serial.print(VFSflowrate);
      // Serial.print(",VFSheat:"); Serial.print(VFSheat,2);
    }

    if (DS18B20_ENABLE) { 
        for(int i=0;i<numberOfDevices; i++)
        {
          if (temps[i]>-50 && temps[i]<80) {
            Serial.print(",T");
            Serial.print(uids[i]);
            Serial.print(":");
            Serial.print(temps[i]);
          }
        }
    }
    
    if (KAMSTRUP_ENABLE && kamstrup_reply_received) {
      // Parse kamstrup mbus data reply
      parse();

      // Print kamstrup data to serial
      Serial.print(",KSflowT:"); Serial.print(KSflowT*0.01,2);
      Serial.print(",KSreturnT:"); Serial.print(KSreturnT*0.01,2);
      Serial.print(",KSdeltaT:"); Serial.print(KSdeltaT*0.01,2);
      Serial.print(",KSflowrate:"); Serial.print(KSflowrate);
      Serial.print(",KSheat:"); Serial.print(KSheat);
      Serial.print(",KSkWh:"); Serial.print(KSkWh);
    }
    Serial.print(",PulseIRDA:"); Serial.print(pulseCount);
    Serial.println();
    delay(100);

    wdt_reset();
  }

  if ((millis()-lastwdtreset)>1000) {
    lastwdtreset = millis();
    wdt_reset();
  }
}

