// HeatpumpMonitor_AutoTemp
// Licence: GPLv3
//
// Compilation note:

// ---------------------------------------------------------
// Compilation 12th December 2017
// ---------------------------------------------------------
// - Arduino 1.8.5

// - JeeLib https://github.com/jcw/jeelib.git 
//   commit: 026bb6ea843abafff1619ae272a4ea754e0b016e

// - EmonLib https://github.com/openenergymonitor/EmonLib.git 
//   commit: bd0c6ac3bea7f098605841d4124b168e311ac51e

// - CustomSoftwareSerial https://github.com/ledongthuc/CustomSoftwareSerial.git 
//   commit: f97d168a6f29c065fd811b137281d35a40a899e6

// - Elster https://github.com/openenergymonitor/ElsterMeterReader.git 
//   commit: e63aa28868de3096723702a95e25f100e5227aaf

// - OneWire https://github.com/PaulStoffregen/OneWire.git
//   commit: ebc5f7f503b7be9c3772c3c131d86d7080cc77cf

// Beware of errors caused by use of previously compiled libraries
// clear complilation cache in /tmp/ if necessary

#include <avr/wdt.h>

#define FirmwareVersion = 4.0
#define DEBUG 0

#define RFM69_ENABLE 1
#define OEM_EMON_ENABLE 1
#define OEM_EMON_ACAC 1
#define DS18B20_ENABLE 1

#define MBUS_ENABLE 1
#define KAMSTRUP_402 // KAMSTRUP_403 or SONTEX_531, 

#define VFS_ENABLE 1
#define ELSTER_IRDA_ENABLE 1

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
#define DS18B20_PIN 19                                                  // Data wire is plugged into port 2 on the Arduino
OneWire ds(DS18B20_PIN);                                           // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)

// Four temperature sensors
byte addr[8]; int uids[4]; double temps[4];
byte numberOfDevices = 0;

// --------------------------------------------------
// Grundfos VFS config
// --------------------------------------------------
const double VFS_maxflow          = 40.0;  // Litres/minute
const double VFS_maxflow_voltage  = 3.5;   // Volts
const double VFS_zeroflow_voltage = 0.25;  // Volts
const double VFS_maxtemp          = 100;   // Celcius
const double VFS_maxtemp_voltage  = 3.5;   // Volts
const double VFS_zerotemp_voltage = 0.5;   // Volts
// --------------------------------------------------
// MBUS
// --------------------------------------------------
byte mbus_address = 100;                   // Set to 0 for auto scan, (test: check timeout 10ms)
int mbus_failures = 0;

#include <CustomSoftwareSerial.h>
CustomSoftwareSerial* customSerial;               // Declare serial

unsigned long last = 0;
unsigned long now = 0;
unsigned long lastwdtreset = 0;

byte bid = 0;
byte bid_end = 255;
byte bid_checksum = 255;
byte len = 0;
byte valid = 0;
byte checksum = 0;

byte bytes[255]; // reduce to 150 if stability problems
// byte dlen = 0;

#define RF_freq RF12_433MHZ                                             // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 1;                                                   // emonTx RFM12B node ID
const int networkGroup = 210;                                           // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLCD needs to be same as emonBase and emonGLCD

#define RF69_COMPAT 1 // set to 1 to use RFM69CW
#include <JeeLib.h>   // make sure V12 (latest) is used if using RFM69CW
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// CT Sensors
int OEMct1 = 0;
int OEMct2 = 0;
long joules_CT1 = 0;
long joules_CT2 = 0;
long CT1_Wh = 0;
long CT2_Wh = 0;

// VFS Variables
double VFSflow = 0;
double VFStemp = 0;

// pulseCounting
long pulseCount = 0;
bool firstrun = true;
unsigned long last_reading = 0;

double VFS_Flow_Cal = 0;
double VFS_Temp_Cal = 0;

unsigned long msgnum = 0;
// -------------------------------------------------------------------
// Elster meter reading
// -------------------------------------------------------------------
void meter_reading(unsigned long r) {
  pulseCount = r;
}

ElsterA100C meter(meter_reading);

// -------------------------------------------------------------------
// onPulse
// The interrupt routine - runs each time a falling edge of a pulse is detected
// -------------------------------------------------------------------
//
void onPulse() {
  pulseCount++;
}

float getTemp(){
  byte data[2];
  ds.reset();
  ds.select(addr);
  ds.write(0x44);
  delay(750);
  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);
  for (int i = 0; i < 2; i++) data[i] = ds.read();
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16;
  return TemperatureSum;
}

// -------------------------------------------------------------------
// Decode
// -------------------------------------------------------------------
long decode_2byte_bin(long i) {
  return bytes[i+0] + (bytes[i+1]<<8);
}

long decode_4byte_bin(long i) {
  return bytes[i+0] + (bytes[i+1]<<8) + (bytes[i+2]<<16) + (bytes[i+3]<<24);
}

float decode_4byte_bin_to_float(long i)
{
  byte float_data[4];
  float_data[0] = bytes[i+0];
  float_data[1] = bytes[i+1];
  float_data[2] = bytes[i+2];
  float_data[3] = bytes[i+3];
  
  union {
      uint32_t u32;
      float f;
  } data;
  memcpy(&(data.u32), float_data, sizeof(uint32_t));
  return data.f;
}


// -------------------------------------------------------------------
// SETUP
// -------------------------------------------------------------------
void setup() {
  wdt_enable(WDTO_8S);
  
  Serial.begin(115200);
  if (DEBUG) Serial.println(F("Startup"));
  if (RFM69_ENABLE) rf12_initialize(nodeID, RF_freq, networkGroup);

  ct1.voltage(0, 262.0, 1.7);
  ct1.current(1, 20.0);
  ct2.voltage(0, 262.0, 1.7);
  ct2.current(2, 20.0);
  
  delay(100);
  
  // MBUS
  //                                     (5,19);
  customSerial = new CustomSoftwareSerial(4,5); // rx, tx
  customSerial->begin(2400, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1

  wdt_reset();

  if (MBUS_ENABLE) {
    // Broadcast set address to 100
    if (DEBUG) Serial.println(F("Set MBUS Address to 100"));
    mbus_set_address(254,mbus_address);
    delay(1000);
    
    #ifdef SONTEX_531
      // Set sontex data reply page to 3
      mbus_request(100,3);
      delay(1000);
    #endif 
  }
  
  wdt_reset();
  if (DEBUG) Serial.println(F("Attached Interrupt"));
  
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
      Serial.print(F("RFM node:"));
      Serial.print(node_id);
      Serial.print(" len:");
      Serial.println(n);
    }

    // If EmonTH on node 23 then decode names fully
    if (node_id == 23 && n == 12)
    {
      emonth = *(PayloadTH*) rf12_data;
      Serial.print(F("THtemp:")); Serial.print(emonth.temp*0.1,1);
      Serial.print(F(",THexttemp:")); Serial.print(emonth.temp_external*0.1,1);
      Serial.print(F(",THhumidity:")); Serial.print(emonth.humidity*0.1,1);
      Serial.print(F(",THbattery:")); Serial.print(emonth.battery*0.1,1);
      Serial.print(F(",THpulse:")); Serial.println(emonth.pulsecount);
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

    // -----------------------------------------------------
    // DS18B20 Temperature sensors
    // -----------------------------------------------------
    byte dsid = 0;
    while(ds.search(addr)) {
  
      unsigned long uid = 0;
      for(byte i=0; i<8; i++) uid += addr[i];
      
      uids[dsid] = uid;
      temps[dsid] = getTemp();
      dsid++;
    }
    numberOfDevices = dsid;

    delay(200);
    
    bool mbus_reply_received = false;
    
    // 1) KAMSTRUP HEAT METER REQUEST:
    if (MBUS_ENABLE) {
      if (DEBUG) Serial.println(F("mbus_request_data"));
      mbus_request_data(mbus_address);
      bid = 0;
  
      int mbus_timeout = 1000;
      unsigned long timer_start = millis();
      while (!mbus_reply_received && (millis()-timer_start)<mbus_timeout)
      {
        while (customSerial->available()) {
          byte val = (byte) customSerial->read();
          bytes[bid] = val;
          
          // Catch ACK
          if (bid==0 && val==0xE5) if (DEBUG) Serial.println(F("ACK"));

          // Long frame start, reset checksum
          if (bid==0 && val==0x68) {
            valid = 1;
            checksum = 0;
          }
      
          // 2nd byte is the frame length
          if (valid && bid==1) {
            len = val;
            bid_end = len+4+2-1;
            bid_checksum = bid_end-1;
          }
          
          if (valid && bid==2 && val!=len) valid = 0;                 // 3rd byte is also length, check that its the same as 2nd byte
          if (valid && bid==3 && val!=0x68) valid = 0;                // 4th byte is the start byte again
          if (valid && bid>3 && bid<bid_checksum) checksum += val;    // Increment checksum during data portion of frame
      
          if (valid && bid==bid_checksum && val!=checksum) valid = 0; // Validate checksum
          if (valid && bid==bid_end && val==0x16) {                   // Parse frame if still valid
            mbus_reply_received = true;
            bid = 0;
            break;
          }
      
          bid++;
        }
      }
  
      if (mbus_reply_received==false) {
        mbus_failures++;
      }
      if (DEBUG) Serial.println(F("MBUS REPLY END"));
    }
    
    wdt_reset();
    delay(200);
    
    // -----------------------------------------------------
    // Analog read for grundfos vortex flow sensor
    // -----------------------------------------------------
    if (VFS_ENABLE) {
      if (DEBUG) Serial.println(F("VFS READ"));
      
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
      
      VFStemp = (A3_voltage - VFS_zerotemp_voltage) / VFS_Temp_Cal;
      if (VFStemp<0) VFStemp = 0;
  
      VFSflow = (A4_voltage - VFS_zeroflow_voltage) / VFS_Flow_Cal;
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
        if (DEBUG) Serial.println(F("OEM CT READ"));
        // Reading of CT sensors needs to go here for stability
        // need to double check the reason for this.
        for (int i=0; i<10; i++) {
          analogRead(0); analogRead(1); analogRead(2);
        }
        delay(200);
        
        if (OEM_EMON_ACAC) {
          ct1.calcVI(30,2000);
          OEMct1 = ct1.realPower;
          wdt_reset();
          ct2.calcVI(30,2000);
          OEMct2 = ct2.realPower;
          wdt_reset();
        } else {
          OEMct1 = 230 * ct1.calcIrms(1480);
          wdt_reset();
          OEMct2 = 230 * ct2.calcIrms(1480);
          wdt_reset();
        }
    
        // Accumulating Watt hours
        int interval = millis() - last_reading;
        last_reading = millis();
    
        if (ct1.realPower > 0 && interval>0) {
          long jouleinc = ct1.realPower * interval *0.001;
          joules_CT1 += jouleinc;
          CT1_Wh += joules_CT1 / 3600;
          joules_CT1 = joules_CT1 % 3600;
        }
    
        if (ct2.realPower > 0 && interval>0) {
          long jouleinc = ct2.realPower * interval *0.001;
          joules_CT2 += jouleinc;
          CT2_Wh += joules_CT2 / 3600;
          joules_CT2 = joules_CT2 % 3600;
        }
        wdt_reset();

        if (DEBUG) Serial.println(F("OEM CT READ END"));
    }

    msgnum++;
    Serial.print(F("Msg:")); Serial.print(msgnum);
    if (OEM_EMON_ENABLE)
    {
        Serial.print(F(",OEMct1:")); Serial.print(OEMct1);
        Serial.print(F(",OEMct2:")); Serial.print(OEMct2);
        Serial.print(F(",OEMct1Wh:")); Serial.print(CT1_Wh);
        Serial.print(F(",OEMct2Wh:")); Serial.print(CT2_Wh);
    }
    
    if (VFS_ENABLE) {
      Serial.print(F(",VFStemp:")); Serial.print(VFStemp,2);
      Serial.print(F(",VFSflow:")); Serial.print(VFSflow,2);
    }

    if (DS18B20_ENABLE) { 
        for(int i=0;i<numberOfDevices; i++)
        {
          if (temps[i]>-50 && temps[i]<80) {
            Serial.print(F(",T"));
            Serial.print(uids[i]);
            Serial.print(F(":"));
            Serial.print(temps[i]);
          }
        }
    }

    if (MBUS_ENABLE && mbus_reply_received) {

      #ifdef KAMSTRUP_402
        Serial.print(F(",KSkWh:")); 
        Serial.print(decode_4byte_bin(27));
        // volume decode_4byte_bin(33)
        // ontime decode_4byte_bin(39)
        Serial.print(F(",KSflowT:")); 
        Serial.print(decode_4byte_bin(45)*0.01,2);
        Serial.print(F(",KSreturnT:"));
        Serial.print(decode_4byte_bin(51)*0.01,2);
        Serial.print(F(",KSdeltaT:")); 
        Serial.print(decode_4byte_bin(57)*0.01,2);
        Serial.print(F(",KSheat:")); 
        Serial.print(decode_4byte_bin(63)*100);
        // maxpower decode_4byte_bin(70)
        Serial.print(F(",KSflowrate:"));
        Serial.print(decode_4byte_bin(75));
        // maxflow decode_4byte_bin(81)
      #endif
      
      #ifdef KAMSTRUP_403
        // Locations set here correspond to location as described in datasheet minus 9 bytes
        byte offset = 9;
        
        Serial.print(F(",KSkWh:"));    
        Serial.print(decode_4byte_bin(21));
        // Cooling E3 decode_4byte_bin(30-offset)
        // Energy E8 decode_4byte_bin(37-offset)
        // Energy E9 decode_4byte_bin(44-offset)
        // Volume decode_4byte_bin(50-9)
        // PulseA decode_4byte_bin(57-offset)
        // PulseB decode_4byte_bin(65-offset)
        // ontime decode_4byte_bin(71-offset)
        Serial.print(F(",KSflowT:")); 
        Serial.print(decode_2byte_bin(83-offset));
        Serial.print(F(",KSreturnT:"));
        Serial.print(decode_2byte_bin(87-offset));
        Serial.print(F(",KSdeltaT:")); 
        Serial.print(decode_2byte_bin(91-offset));
        Serial.print(F(",KSheat:"));
        Serial.print(decode_4byte_bin(95-offset));
        Serial.print(F(",KSflowrate:"));
        Serial.print(decode_4byte_bin(107-offset));
      #endif
    
      #ifdef SONTEX_531
        Serial.print(F(",SNXflowT:"));
        Serial.print(decode_4byte_bin_to_float(21));
        Serial.print(F(",SNXreturnT:"));
        Serial.print(decode_4byte_bin_to_float(27));
        Serial.print(F(",SNXflowrate:"));
        Serial.print(decode_4byte_bin(33));
        Serial.print(F(",SNXheat:"));
        Serial.print(decode_4byte_bin(39));
      #endif
    }

    if (pulseCount>0) {
      Serial.print(F(",PulseIRDA:")); Serial.print(pulseCount);
    }
    Serial.println();
    delay(100);

    wdt_reset();
  }

  if ((millis()-lastwdtreset)>1000) {
    lastwdtreset = millis();
    wdt_reset();
  }
}

