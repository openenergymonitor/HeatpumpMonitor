# CustomSoftwareSerial
Alternative SoftwareSerial in Arduino. CustomSoftwareSerial library allow you configure and custom parity bit, stop bits and number of data bits. Hope it useful for you.

You can download from  [v1.0.tar.gz](https://github.com/ledongthuc/CustomSoftwareSerial/releases/download/V1.0/CustomSoftwareSerial_v1.0.tar.gz) or [v1.0.zip](https://github.com/ledongthuc/CustomSoftwareSerial/releases/download/V1.0/CustomSoftwareSerial_v1.0.zip)

# Install
After downloading the package, follow step in http://www.arduino.cc/en/guide/libraries

# Quick tutorial
Simple demo to use CustomSoftwareSerial
```cpp
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
```

# Supported Configuration
We support configurations to setup stop bit, parity bit and number of data bits:

| Name        | Number of data bits | Parity bit | Number of stop bit |
|:-----------:|:-------------------:|:----------:|:------------------:|
| CSERIAL_5N1 | 5                   | None       | 1                  |
| CSERIAL_6N1 | 6                   | None       | 1                  |
| CSERIAL_7N1 | 7                   | None       | 1                  |
| CSERIAL_8N1 | 8                   | None       | 1                  |
| CSERIAL_5N2 | 5                   | None       | 2                  |
| CSERIAL_6N2 | 6                   | None       | 2                  |
| CSERIAL_7N2 | 7                   | None       | 2                  |
| CSERIAL_8N2 | 8                   | None       | 2                  |
| CSERIAL_5O1 | 5                   | Odd        | 1                  |
| CSERIAL_6O1 | 6                   | Odd        | 1                  |
| CSERIAL_7O1 | 7                   | Odd        | 1                  |
| CSERIAL_8O1 | 8                   | Odd        | 1                  |
| CSERIAL_5O2 | 5                   | Odd        | 2                  |
| CSERIAL_6O2 | 6                   | Odd        | 2                  |
| CSERIAL_7O2 | 7                   | Odd        | 2                  |
| CSERIAL_8O2 | 8                   | Odd        | 2                  |
| CSERIAL_5E1 | 5                   | Even       | 1                  |
| CSERIAL_6E1 | 6                   | Even       | 1                  |
| CSERIAL_7E1 | 7                   | Even       | 1                  |
| CSERIAL_8E1 | 8                   | Even       | 1                  |
| CSERIAL_5E2 | 5                   | Even       | 2                  |
| CSERIAL_6E2 | 6                   | Even       | 2                  |
| CSERIAL_7E2 | 7                   | Even       | 2                  |
| CSERIAL_8E2 | 8                   | Even       | 2                  |

# Testing
## Test Configuration
Testing demo to test configuration's values
```cpp
#include <CustomSoftwareSerial.h>

CustomSoftwareSerial* customSerial;

void setup() {
  Serial.begin(9600);
  
  Serial.println("**** DEFAULT ****");
  customSerial = new CustomSoftwareSerial(9, 10);
  printPort();
  
  Serial.println("**** CSERIAL_5N1 ****");
  customSerial->begin(1200, CSERIAL_5N1);
  printPort();
  
  Serial.println("**** CSERIAL_6N1 ****");
  customSerial->begin(1200, CSERIAL_6N1);
  printPort();
  
  Serial.println("**** CSERIAL_7N1 ****");
  customSerial->begin(1200, CSERIAL_7N1);
  printPort();
  
  Serial.println("**** CSERIAL_8N1 ****");
  customSerial->begin(1200, CSERIAL_8N1);
  printPort();
  
  Serial.println("**** CSERIAL_5N2 ****");
  customSerial->begin(1200, CSERIAL_5N2);
  printPort();
  
  Serial.println("**** CSERIAL_6N2 ****");
  customSerial->begin(1200, CSERIAL_6N2);
  printPort();
  
  Serial.println("**** CSERIAL_7N2 ****");
  customSerial->begin(1200, CSERIAL_7N2);
  printPort();
  
  Serial.println("**** CSERIAL_8N2 ****");
  customSerial->begin(1200, CSERIAL_8N2);
  printPort();
  
  Serial.println("**** CSERIAL_5O1 ****");
  customSerial->begin(1200, CSERIAL_5O1);
  printPort();
  
  Serial.println("**** CSERIAL_6O1 ****");
  customSerial->begin(1200, CSERIAL_6O1);
  printPort();
  
  Serial.println("**** CSERIAL_7O1 ****");
  customSerial->begin(1200, CSERIAL_7O1);
  printPort();
  
  Serial.println("**** CSERIAL_8O1 ****");
  customSerial->begin(1200, CSERIAL_8O1);
  printPort();
  
  Serial.println("**** CSERIAL_5O2 ****");
  customSerial->begin(1200, CSERIAL_5O2);
  printPort();
  
  Serial.println("**** CSERIAL_6O2 ****");
  customSerial->begin(1200, CSERIAL_6O2);
  printPort();
  
  Serial.println("**** CSERIAL_7O2 ****");
  customSerial->begin(1200, CSERIAL_7O2);
  printPort();
  
  Serial.println("**** CSERIAL_8O2 ****");
  customSerial->begin(1200, CSERIAL_8O2);
  printPort();
  
  Serial.println("**** CSERIAL_5E1 ****");
  customSerial->begin(1200, CSERIAL_8O2);
  printPort();
  
  Serial.println("**** CSERIAL_6E1 ****");
  customSerial->begin(1200, CSERIAL_6E1);
  printPort();
  
  Serial.println("**** CSERIAL_7E1 ****");
  customSerial->begin(1200, CSERIAL_7E1);
  printPort();
  
  Serial.println("**** CSERIAL_8E1 ****");
  customSerial->begin(1200, CSERIAL_8E1);
  printPort();
  
  Serial.println("**** CSERIAL_5E2 ****");
  customSerial->begin(1200, CSERIAL_5E2);
  printPort();
  
  Serial.println("**** CSERIAL_6E2 ****");
  customSerial->begin(1200, CSERIAL_6E2);
  printPort();
  
  Serial.println("**** CSERIAL_7E2 ****");
  customSerial->begin(1200, CSERIAL_7E2);
  printPort();
  
  Serial.println("**** CSERIAL_8E2 ****");
  customSerial->begin(1200, CSERIAL_8E2);
  printPort();  
}

void loop() {
  
}

void printPort() {
  Serial.println("Port's information of CustomSoftwareSerial");

  Serial.print("- Number of data bit: ");
  Serial.println(customSerial->getNumberOfDataBit());
  
  Parity parity = customSerial->getParity();
  char* parityText;
  switch(parity) {
    case NONE:
      parityText = "None";
      break;
    case ODD:
      parityText = "Odd";
      break;
    case EVEN:
      parityText = "Even";
      break;
  }
  Serial.print("- Parity bit: ");
  Serial.println(parityText);
  
  
  Serial.print("- Number of stop bit: ");
  Serial.println(customSerial->getNumberOfStopBit());

  Serial.println("");
}
```

# Little story
A sunny day, I implement an library to control BLE HM-10 module through serial port but I recognize the Arduino's SoftwareSerial doesn't support parity bit. The stop bit only support one. So, I create and share this project for someone who need it. 

If you got any problem or question, let me know. Thanks
