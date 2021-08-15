// Energy Monitoring only example for heatpump monitor board.
// Licence: GPLv3
#include <avr/wdt.h>

#define FirmwareVersion = 1.0
#define DEBUG 0
#define emonTxV3 1
#define PULSE_ENABLE 0

#include "EmonLib.h"                   // Include Emon Library:  https://github.com/openenergymonitor/EmonLib
EnergyMonitor ct1;                     // Create an instance
EnergyMonitor ct2;                     // Create an instance

unsigned long last = 0;
unsigned long now = 0;
unsigned long lastwdtreset = 0;

// pulseCounting
long pulseCount = 0;

int joules_CT1 = 0;
int joules_CT2 = 0;
unsigned long CT1_Wh = 0;
unsigned long CT2_Wh = 0;
unsigned long msgnum = 0;

bool firstrun = true;
unsigned long last_reading = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Startup");

  ct1.voltage(0, 262.0, 1.7);
  ct1.current(1, 90.9);
  ct2.voltage(0, 262.0, 1.7);
  ct2.current(2, 90.9);

  delay(100);
  if (PULSE_ENABLE) attachInterrupt(1, onPulse, FALLING);

  CT1_Wh = 0;
  CT2_Wh = 0;

  wdt_enable(WDTO_8S);
  wdt_reset();

}

void loop() {
  now = millis();

  if ((now - last) >= 9800 || firstrun) {
    wdt_reset();
    last = now; firstrun = false;

    // Reading of CT sensors needs to go here for stability
    // need to double check the reason for this.
    for (int i = 0; i < 10; i++) {
      analogRead(0); analogRead(1); analogRead(2);
    }
    delay(200);
    ct1.calcVI(30, 2000);
    wdt_reset();
    int OEMct1 = ct1.realPower;
    ct2.calcVI(30, 2000);
    wdt_reset();
    int OEMct2 = ct2.realPower;

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

    msgnum++;
    Serial.print("Msg:"); Serial.print(msgnum);
    Serial.print(",OEMct1:"); Serial.print(OEMct1);
    Serial.print(",OEMct2:"); Serial.print(OEMct2);
    Serial.print(",OEMct1Wh:"); Serial.print(CT1_Wh);
    Serial.print(",OEMct2Wh:"); Serial.print(CT2_Wh);
    if (PULSE_ENABLE) {
      Serial.print(",PulseCount:"); Serial.print(pulseCount);
    }
    Serial.println();
    delay(200);
  }

  if ((millis() - lastwdtreset) > 1000) {
    lastwdtreset = millis();
    wdt_reset();
  }
}

// The interrupt routine - runs each time a falling edge of a pulse is detected
void onPulse()
{
  pulseCount++;                                               // count pulse
}
