#include <CustomSoftwareSerial.h>

CustomSoftwareSerial* customSerial;               // Declare serial

// Init value
void setup() {
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.write("Hello World");

  customSerial = new CustomSoftwareSerial(9, 10); // rx, tx
  customSerial->begin(9600, CSERIAL_8N1);         // Baud rate: 9600, configuration: CSERIAL_8N1
  customSerial->write("Test message");            // Write testing data
}

void loop() {
  if (customSerial->available())
    Serial.write(customSerial->read());
  if (Serial.available())
    customSerial->write(Serial.read());

  delay(1000);
}
