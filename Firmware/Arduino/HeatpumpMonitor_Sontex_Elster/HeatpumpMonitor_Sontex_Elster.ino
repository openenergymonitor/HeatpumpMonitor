// Works with Arduino 1.6.7 but not 1.8.2

#include <avr/wdt.h>

#define FirmwareVersion = 2.0
#define DEBUG 0
#define RFM69_ENABLE 1
#define DS18B20_ENABLE 1
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
#include <OneWire.h>

#define DS18B20_PIN 19                                                  // Data wire is plugged into port 2 on the Arduino
OneWire ds(DS18B20_PIN);                                           // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)

// Four temperature sensors
byte addr[8]; int uids[4]; double temps[4];
byte numberOfDevices = 0;

// --------------------------------------------------
// MBUS config
// --------------------------------------------------

int mbus_address = -1;                   // Set to 0 for auto scan, (test: check timeout 10ms)
int mbus_failures = 0;
#include <CustomSoftwareSerial.h>
CustomSoftwareSerial* customSerial;               // Declare serial

unsigned long last = 0;
unsigned long now = 0;
unsigned long lastwdtreset = 0;

int bid = 0;
byte bytes[82];
byte dlen = 0;

#define RF_freq RF12_433MHZ                                             // Frequency of RF12B module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
const int nodeID = 1;                                                   // emonTx RFM12B node ID
const int networkGroup = 210;                                           // emonTx RFM12B wireless network group - needs to be same as emonBase and emonGLCD needs to be same as emonBase and emonGLCD

#define RF69_COMPAT 1 // set to 1 to use RFM69CW
#include <JeeLib.h>   // make sure V12 (latest) is used if using RFM69CW
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// pulseCounting
long pulseCount = 0;

bool firstrun = true;
unsigned long last_reading = 0;
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

float getTemp(byte* addr){
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
// SETUP
// -------------------------------------------------------------------
void setup() {
  wdt_enable(WDTO_8S);
  
  Serial.begin(115200);
  if (DEBUG) Serial.println("Startup");
  if (RFM69_ENABLE) rf12_initialize(nodeID, RF_freq, networkGroup);
  
  delay(100);
  
  // MBUS
  //                                     (5,19);
  customSerial = new CustomSoftwareSerial(4,5); // rx, tx
  customSerial->begin(2400, CSERIAL_8E1);         // Baud rate: 9600, configuration: CSERIAL_8N1

  wdt_reset();

  if (MBUS_ENABLE)
  {
    if (mbus_address==-1) {
      for (int i=0; i<3; i++) 
      {
        if (DEBUG) Serial.println("Scanning MBUS ");
        mbus_normalize();
        delay(100);
        mbus_address = mbus_scan();
        if (mbus_address!=-1) {
          if (DEBUG) Serial.print("Meter found, address: ");
          if (DEBUG) Serial.println(mbus_address);
          break;
        } else {
          if (DEBUG) Serial.println("No MBUS meter found");
        }
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

  wdt_reset();
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

    delay(100);
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
      temps[dsid] = getTemp(addr);
      dsid++;
    }
    numberOfDevices = dsid;

    delay(200);
    
    bool mbus_reply_received = false;
    
    if (MBUS_ENABLE) {
      if (DEBUG) Serial.println("mbus_request_data");
      if (mbus_address>0) {
        mbus_request_data(mbus_address);
      } else {
        if (mbus_failures>10) {
          if (DEBUG) Serial.println("Mbus scan start");
          mbus_address = mbus_scan();
          mbus_failures = 0;
          wdt_reset();
        }
      }
      bid = 0;
  
      int mbus_timeout = 1000;
      unsigned long timer_start = millis();
      while (!mbus_reply_received && (millis()-timer_start)<mbus_timeout)
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

          if (bid==81) // or 120
          {
            mbus_reply_received = true;
            bid = 0;
          }
        }
      }
  
      if (mbus_reply_received==false) {
        mbus_failures++;
      }
    }

    wdt_reset();

    delay(100);

    msgnum++;
    Serial.print("Msg:"); Serial.print(msgnum);
    
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
    
    if (MBUS_ENABLE && mbus_reply_received) {
      // Parse kamstrup mbus data reply
      Serial.print(",SNXerror:");
      Serial.print(decode_4byte_bin(27));
      Serial.print(",SNXenergy:");
      Serial.print(decode_4byte_bin(39));
      Serial.print(",SNXvol1:");
      Serial.print(decode_4byte_bin(45));
      Serial.print(",SNXvol2:");
      Serial.print(decode_4byte_bin(59));
    }
    Serial.print(",Elster:"); Serial.print(pulseCount);
    Serial.println();
    delay(100);

    wdt_reset();
  }

  if ((millis()-lastwdtreset)>1000) {
    lastwdtreset = millis();
    wdt_reset();
  }
}

