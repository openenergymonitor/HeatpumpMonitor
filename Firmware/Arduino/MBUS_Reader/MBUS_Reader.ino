// --------------------------------------------------------
// Arduino MBUS reader
//
// Part of the OpenEnergyMonitor.org project
// Licence: GPL
// Author: Trystan Lea
// --------------------------------------------------------

#define DIF  0
#define DIFE 1
#define VIF  2
#define VIFE 3
#define DATA 4

#include <CustomSoftwareSerial.h>
CustomSoftwareSerial* customSerial;               // Declare serial
byte address = 16;

byte bid = 0;
byte bid_end = 255;
byte bid_checksum = 255;
byte len = 0;
byte valid = 0;
byte checksum = 0;
byte type = DIF;
byte data_count = 0;
byte data_len = 0;

byte cmd[10];
byte cmdi = 0;
byte page = 0;

byte data[150];

// --------------------------------------------------------
// Setup 
// --------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Arduino MBUS reader");
  Serial.println("- a: set mbus address to 100");
  Serial.println("- n: normalize");
  Serial.println("- r: request data");
  Serial.println("- c: application reset");
  Serial.println("- i: set id 1234");
  Serial.println("- b: set baudrate 4800");
  Serial.println("- p: select page eg: p1");
  Serial.println();
  customSerial = new CustomSoftwareSerial(4,5);
  customSerial->begin(4800, CSERIAL_8E1);
}
// --------------------------------------------------------
// Parse command
// --------------------------------------------------------
void parse_cmd()
{
  // for (byte c=0; c<cmdi; c++) Serial.print((char)cmd[c]);

  switch(cmd[0])
  {
    case 'p':
      Serial.println("Command sent: page");
      page = 0; 
      if (cmdi==2) page = cmd[1]-48;
      if (cmdi==3) page = (cmd[1]-48)*10 + (cmd[2]-48);
      Serial.print("Page: ");
      Serial.println(page);
      mbus_request(100,page);
      break;
    case 'r':
      Serial.println("Command sent: request data");
      mbus_request_data(100);
      break;
    case 'n':
      Serial.println("Command sent: normalize");
      mbus_normalize(100);
      break;
    case 'c':
      Serial.println("Command sent: application reset");
      mbus_application_reset(100);
      break;
    case 'i':
      Serial.println("Command sent: set id");
      mbus_set_id(100);
      break;
    case 'b':
      Serial.println("Command sent: set baudrate");
      // B8 Baudrate 300
      // B9 Baudrate 600
      // BA Baudrate 1200
      // BB Baudrate 2400
      // BC Baudrate 4800
      // BD Baudrate 9600
      mbus_set_baudrate(100,0xBC);
      break;
    case 'a':
      Serial.println("Command sent: set address 100");
      // Address 1-250, 254,255 broadcast, 0 unconfigured
      mbus_set_address(254,100);
      break;

  }

  bid = 0;
}

// --------------------------------------------------------
// Parse returned frame
// --------------------------------------------------------
void parse_frame()
{
  byte bid = 0;
  for (byte i=0; i<(len+6); i++)
  {
    byte val = data[i];

    Serial.print(i);
    Serial.print("\t");
    Serial.print(val);
    Serial.print("\t");
    Serial.print(val,HEX);
    Serial.print("\t");
    
    if (bid==0 && val==0x68) {
      Serial.print("START");
      valid = 1;
      checksum = 0;
      type = DIF;
    }

    if (valid && bid==1) {
      len = val;
      bid_end = len+4+2-1;
      bid_checksum = bid_end-1;
      Serial.print("LEN");
    }
    if (valid && bid==2) {
      if (val==len) {
        Serial.print("LEN");
      } else valid = 0;
    }

    if (valid && bid==3) {
      if (val==0x68) {
        Serial.print("START");
      } else valid = 0;
    }
    
    if (valid)
    {
      if (bid==4) Serial.print("C FIELD");
      if (bid==5) Serial.print("ADDRESS");
      if (bid==6) Serial.print("CI FIELD");
      if (bid==7 || bid==8 || bid==9 || bid==10) Serial.print("ID");
      if (bid==11 || bid==12) Serial.print("MID");
      if (bid==13) Serial.print("Gen");
      if (bid==14) Serial.print("Media");
      if (bid==15) Serial.print("Access");
      if (bid==16) Serial.print("Access");
      if (bid==17 || bid==18) Serial.print("Signature");
      
      if (bid>18) {
        if (type==DIF) {
          Serial.print("DIF ");
          if (val>=0x80) type=DIFE; else type=VIF;
          data_count = 0;

          byte data_field = val & 0x0F; // AND logic
          
          switch (data_field) {
            case 0: data_len = 0; break;
            case 1: data_len = 1; break;       
            case 2: data_len = 2; break;  
            case 3: data_len = 3; break;
            case 4: data_len = 4; break;                  
            case 5: data_len = 4; break;   
            case 6: data_len = 6; break;  
            case 7: data_len = 8; break;
            case 8: data_len = 0; break; 
            case 9: data_len = 1; break; 
            case 10: data_len = 2; break; 
            case 11: data_len = 3; break; 
            case 12: data_len = 4; break; 
            case 13: data_len = 0; break; 
            case 14: data_len = 6; break; 
            case 15: data_len = 0; break;         
          }
                       
          Serial.print(data_len);
        }

        else if (type==DIFE) {
          Serial.print("DIFE");
          if (val>=0x80) type=DIFE; else type=VIF;
        }

        else if (type==VIF) {
          Serial.print("VIF ");
          if (val>=0x80) type=VIFE; else type=DATA;

          if (val==0x06) Serial.print("Energy kWh");
          if (val==0x13) Serial.print("Volume 0.001m3");
          if (val==0x14) Serial.print("Volume 0.01m3");

          if (val==0x5b) Serial.print("Flow Temp C");
          if (val==0x5f) Serial.print("Return Temp C");
          if (val==0x3e) Serial.print("Volume Flow m3/h");
          if (val==0x2b) Serial.print("Power W");
          if (val==0x22) Serial.print("Power 100 J/h");
          if (val==0x6D) Serial.print("TIME & DATE");

          if (val==0x70) Serial.print("Duration seconds");
          if (val==0x75) Serial.print("Duration minutes");
          if (val==0x78) Serial.print("Fab No");
          if (val==0x79) Serial.print("Enhanced");
          
        }

        else if (type==VIFE) {
          Serial.print("VIFE");
          if (val>=0x80) type=VIFE; else type=DATA;
        }

        else if (type==DATA) {
          Serial.print("DATA");
          data_count++;
          if (data_count==data_len) {
            type=DIF;
          }
        }
        
      }

      if (bid>3 && bid<bid_checksum) checksum += val;
    }

    if (valid && bid==bid_checksum) {
      if (val==checksum) {
        Serial.print("CHECKSUM VALID");
      } else {
        Serial.print("CHECKSUM INVALID");
        valid = 0;
      }
    }
    
    if (valid && bid==bid_end && val==0x16) Serial.print("END");

    Serial.println();
    bid++;
  }
  
}

// --------------------------------------------------------
// Loop 
// --------------------------------------------------------
void loop() {

  if (Serial.available()) {
    byte c = Serial.read();

    if (c=='\n' || c=='\r') {
      if (cmdi>0) parse_cmd();
      cmdi = 0;
    } else {
      cmd[cmdi++] = c;
    }
  }

  while(customSerial->available())
  {
    byte val = (byte) customSerial->read();
    data[bid] = val;

    // Catch ACK
    if (bid==0 && val==0xE5) Serial.println("ACK");

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
    if (valid && bid==bid_end && val==0x16) parse_frame();      // Parse frame if still valid

    bid++;
  }
}


