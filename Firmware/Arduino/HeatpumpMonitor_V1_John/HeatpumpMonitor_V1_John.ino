// Swift, 1bird Heatpump monitor example with:
// MBUS meter reader for kampstrup multical 402
// Grundfos vortex flow sensor on analog 0 and 1
// several x DS18B20 flow + return sensors
// All sending via RFM12/69 to group 219, then converted to 217
// Licence: GPLv3
#include <avr/wdt.h>  

#define FirmwareVersion = 1.02
#define DEBUG 0
#define emonTxV3 1
#define SEND_RFM69_DATA 1

// ------------------------------------------------------------------------------------------
// Datastructure for data sent via RFM12 or RFM69 radio module - alternative path to ESP WIFI
// ------------------------------------------------------------------------------------------
/*

Copy this node decoder to your emonhub.conf file to decode the heatpump monitor rfm69 data packet:

[[1]]
    nodename = Heatpump Monitor
    firmware = v0.1
    hardware = v0.1
    [[[rx]]]
       names = OEMct1,OEMct2,DSairoutT,DSairinT,DSflowT,DSreturnT,VFSflowT,VFSflowrate,VFSheat,KSflowT,KSreturnT,KSdeltaT,KSflowrate,KSheat,KSheat2,KSkWh,pulseCount,T1,T2,T3
       datacodes = h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,L,L,h,h,h
       scales = 1,1,0.01,0.01,0.01,0.01,0.01,1,1,0.01,0.01,0.01,1,1,1,1,1,0.01,0.01,0.01
       units = W,W,C,C,C,C,C,L/h,W,C,C,K,L/h,W,W,kWh,Wh,C,C,C
*/

// RFM Packet structure
typedef struct {
  // Integer datatypes:
    // OpenEnergyMonitor CTs
      int OEMct1;
      int OEMct2;
    // DS18B20 temperatures:
      int DSairoutT;
      int DSairinT;
      int DSflowT;
      int DSreturnT;
    // Grundfos VFS flow meter
      int VFSflowT;
      int VFSflowrate;
      int VFSheat;
    // Kamstrup heat meter
      int KSflowT; 
      int KSreturnT;
      int KSdeltaT;
      int KSflowrate;
      int KSheat;
      int KSheat2;            // have added this as a corrected value to allow for defrosts.  It makes heat during defrost as a negative value.
        
  // Long data types:
    long KSkWh;
    long pulseCount;
    long KSpulse;

      int T1;    /// added to test pockets
      int T2;
      int T3;
} PayloadTX;         // neat way of packaging data for RF comms
PayloadTX emontx;
// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------

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

#include "EmonLib.h"                   // Include Emon Library:  https://github.com/openenergymonitor/EmonLib
EnergyMonitor ct1;                     // Create an instance
EnergyMonitor ct2;                     // Create an instance

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 19                                                  // Data wire is plugged into port 2 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);                                          // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);                                    // Pass our oneWire reference to Dallas Temperature.
DeviceAddress sensor;

// --------------------------------------------------
// Grundfos VFS config
// --------------------------------------------------
double VFS_maxflow          = 40.0;  // Litres/minute (replace old Grundfoss 100 for Sika 40, 25th May 2016)
double VFS_maxflow_voltage  = 3.5;   // Volts   (0.5 v at 2 lit/min equates to 0.35v at zero flow
double VFS_zeroflow_voltage = 0.35;  // Volts
double VFS_maxtemp          = 100;   // Celcius
double VFS_maxtemp_voltage  = 3.5;   // Volts
double VFS_zerotemp_voltage = 0.5;   // Volts
// --------------------------------------------------
// Kamstrup MBUS config
// --------------------------------------------------
byte kamstrup_mbus_address = 0;                  // Set to 0 for auto scan, (test: check timeout 10ms)
int kamstrup_failures = 0;
#include <CustomSoftwareSerial.h>
CustomSoftwareSerial* customSerial;               // Declare serial

unsigned long last = 0;
unsigned long now = 0;
unsigned long lastwdtreset = 0;

int bid = 0;
byte bytes[120];
byte dlen = 0;

#define RF_freq RF12_433MHZ                                             // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 13;                                                   // emonTx RFM12B node ID
const int networkGroup = 219;                                           // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLCD needs to be same as emonBase and emonGLCD

#define RF69_COMPAT 1 // set to 1 to use RFM69CW 
#include <JeeLib.h>   // make sure V12 (latest) is used if using RFM69CW
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// pulseCounting   WHY IS BELOW COMMENTED OUT?
// pulseCounting
long pulseCount = 0;
//unsigned long pulseTime,lastTime; // Used to measure time between pulses
//double power;
//int ppwh = 1;                     // pulses per watt hour - found or set on the meter.

int joules_CT1 = 0;
int joules_CT2 = 0;
unsigned long CT1_Wh = 0;
unsigned long CT2_Wh = 0;

bool firstrun = true;
unsigned long last_reading = 0;

void setup() {
  wdt_enable(WDTO_8S);
  
  Serial.begin(115200);
  Serial.println("Startup");
  rf12_initialize(nodeID, RF_freq, networkGroup);
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
  wdt_reset();
  if (DEBUG) Serial.println("Attached Interrupt");
  delay(100);
  attachInterrupt(1, onPulse, FALLING);

  CT1_Wh = 0;
  CT2_Wh = 0;
}

void loop() {
  now = millis();
  
  if ((now-last)>=9800 || firstrun) {
    wdt_reset();
    
    last = now; firstrun = false;

    // 1) KAMSTRUP HEAT METER REQUEST: 

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

    bool kamstrup_reply_received = false;
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
        
        if (bid==100)
        {
          kamstrup_reply_received = true;
          bid = 0;
        }
      }
    }

    if (kamstrup_reply_received==false) {
      kamstrup_failures++;
    }

    wdt_reset();

    delay(200);
    // DS18B20 temp sensors
    sensors.requestTemperatures();
    
    emontx.DSairinT = sensors.getTempCByIndex(0)*100;
    emontx.DSairoutT = sensors.getTempCByIndex(1)*100;
    emontx.DSflowT = (sensors.getTempCByIndex(2) + 0.356)*100 ;    // .35 epoxy march 2016  sensor calibration for testing sensor fixing methods
    emontx.DSreturnT = sensors.getTempCByIndex(5)*100;             //was3
    emontx.T1 = (sensors.getTempCByIndex(4) + 0.305 )*100;        // .40 T1 short               //march 2016  sensor calibration for testing sensor fixing methods 
    emontx.T2 = (sensors.getTempCByIndex(3) + 0.294 )*100;   //  .29 long was5                     //march 2016  sensor calibration for testing sensor fixing methods
    emontx.T3 = (sensors.getTempCByIndex(6)  + 0.199 )*100;   // .15 return
    
    // -----------------------------------------------------
    // Analog read for grundfos vortex flow sensor
    // -----------------------------------------------------
    delay(200);
    unsigned long sumA3 = 0;
    unsigned long sumA4 = 0; 
    for (int i=0; i<100; i++) {
      sumA3 += analogRead(3);
      sumA4 += analogRead(4);
    }
    double A3 = (sumA3 / 100.0);
    double A4 = (sumA4 / 100.0);
    double A3_voltage = 3.3*(A3/1023.0);
    double A4_voltage = 3.3*(A4/1023.0);
    double VFSflow = (A4_voltage - VFS_zeroflow_voltage) / ((VFS_maxflow_voltage-VFS_zeroflow_voltage)/VFS_maxflow);
    if (VFSflow<0) VFSflow = 0;
    if (A4_voltage<0.4) VFSflow = 0;  // Minimum voltage to accept flow reading, datasheet recommends 0.5V
    
    double VFStemp = (A3_voltage - VFS_zerotemp_voltage) / ((VFS_maxtemp_voltage-VFS_zerotemp_voltage)/VFS_maxtemp);
    if (VFStemp<0) VFStemp = 0;

    emontx.VFSflowT = VFStemp*100;
    emontx.VFSflowrate = VFSflow*60;

    double deltaT = (1.0*emontx.DSflowT-emontx.DSreturnT)*0.01;
    double VFSheat = ((VFSflow/60.0)*4181.0) * deltaT;
    emontx.VFSheat = VFSheat;     // do we need to cap/limit the heat values here??

    wdt_reset();
    
    // Reading of CT sensors needs to go here for stability 
    // need to double check the reason for this.
    for (int i=0; i<10; i++) {
      analogRead(0); analogRead(1); analogRead(2);
    }
    delay(200);
    ct1.calcVI(30,2000);
    emontx.OEMct1 = ct1.realPower;
    ct2.calcVI(30,2000);
    emontx.OEMct2 = ct2.realPower;

    // Accumulating Watt hours
    int interval = millis() - last_reading;
    last_reading = millis();

    if (ct1.realPower>0) {
        joules_CT1 += (ct1.realPower * interval * 0.001);
        CT1_Wh += joules_CT1 / 3600;
        joules_CT1 = joules_CT1 % 3600;
    }

    if (ct2.realPower>0) {
        joules_CT2 += (ct2.realPower * interval * 0.001);
        CT2_Wh += joules_CT2 / 3600;
        joules_CT2 = joules_CT2 % 3600;
    }

    emontx.pulseCount = pulseCount;

    Serial.print("OEMct1:"); Serial.print(emontx.OEMct1);
    Serial.print(",OEMct2:"); Serial.print(emontx.OEMct2);
    Serial.print(",OEMct1Wh:"); Serial.print(CT1_Wh);
    Serial.print(",OEMct2Wh:"); Serial.print(CT2_Wh);    

    Serial.print(",DSairoutT:"); Serial.print(emontx.DSairoutT*0.01,2);
    Serial.print(",DSairinT:"); Serial.print(emontx.DSairinT*0.01,2);
    Serial.print(",DSflowT:"); Serial.print(emontx.DSflowT*0.01,2);
    Serial.print(",DSreturnT:"); Serial.print(emontx.DSreturnT*0.01,2);
    
    Serial.print(",VFSflowT:"); Serial.print(emontx.VFSflowT*0.01);
    Serial.print(",VFSflowrate:"); Serial.print(emontx.VFSflowrate);
    Serial.print(",VFSheat:"); Serial.print(VFSheat,2);

    // Serial.print(",T1:"); Serial.print(emontx.T1*0.01,2);   // temporary temperures for testing pipe pockets etc
    //  Serial.print(",T2:"); Serial.print(emontx.T2*0.01,2);
    //  Serial.print(",T3:"); Serial.println(emontx.T3*0.01,2);
  
    if (kamstrup_reply_received) {
      // Parse kamstrup mbus data reply
      parse();

      // Print kamstrup data to serial
      Serial.print(",KSflowT:"); Serial.print(emontx.KSflowT*0.01,2);
      Serial.print(",KSreturnT:"); Serial.print(emontx.KSreturnT*0.01,2);
      Serial.print(",KSdeltaT:"); Serial.print(emontx.KSdeltaT*0.01,2);
      Serial.print(",KSflowrate:"); Serial.print(emontx.KSflowrate);
      Serial.print(",KSheat:"); Serial.print(emontx.KSheat);
      Serial.print(",KSkWh:"); Serial.print(emontx.KSkWh);
      Serial.print(",KSpulse:"); Serial.print(emontx.KSpulse);
    }

    Serial.print(",PulseCount:"); Serial.print(emontx.pulseCount);
    Serial.println();
    delay(100);

    if (SEND_RFM69_DATA) {
      if (DEBUG) Serial.println("RFM send");
      // if ready to send + exit loop if it gets stuck as it seems too
      int rf = 0; while (!rf12_canSend() && rf<10) {rf12_recvDone(); rf++;}
      rf12_sendStart(0, &emontx, sizeof emontx);
      // set the sync mode to 2 if the fuses are still the Arduino default
      // mode 3 (full powerdown) can only be used with 258 CK startup fuses
      rf12_sendWait(2);
    
      delay(100);
      wdt_reset();
    }
  }

  if ((millis()-lastwdtreset)>1000) {
    lastwdtreset = millis();
    wdt_reset();
  }
}

void parse()
{
  byte i=25;
  emontx.KSkWh = decodeval(i); i+=6;
  int volume = decodeval(i); i+=6;
  int ontime = decodeval(i); i+=6;
  emontx.KSflowT = decodeval(i); i+=6;
  emontx.KSreturnT = decodeval(i); i+=6;
  emontx.KSdeltaT = decodeval(i); i+=6;
  emontx.KSheat = decodeval(i)*100; i+=6;
  int maxpower = decodeval(i)*100; i+=6;
  emontx.KSflowrate = decodeval(i); i+=6;
  int maxflow = decodeval(i); i+=18;
  emontx.KSpulse = decodeval(i);
  
  // Correction for ASHP defrosts. Kamstrup normally show 'cooling' as a positive number.  
  //If dt is positive, KSheat2 = KSheat, BUT if dt is -ve (defrost), KSheat2 is a negative number.
  if (emontx.KSflowT>emontx.KSreturnT) emontx.KSheat2 = emontx.KSheat; else  emontx.KSheat2 = -1 * emontx.KSheat;
}

long decodeval(long i) {
  return bytes[i+2] + (bytes[i+3]<<8) + (bytes[i+4]<<16) + (bytes[i+5]<<24);
}

// The interrupt routine - runs each time a falling edge of a pulse is detected
void onPulse()                  
{
  //lastTime = pulseTime;
  //pulseTime = micros();
  pulseCount++;                                               // count pulse               
  // power = int((3600000000.0 / (pulseTime - lastTime))/ppwh);  // calculate power
}
