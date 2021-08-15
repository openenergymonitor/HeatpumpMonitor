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
