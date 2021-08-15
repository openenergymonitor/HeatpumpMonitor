// Licence: GPL v3
#define MBUS_FRAME_SHORT_START          0x10
#define MBUS_FRAME_LONG_START           0x68
#define MBUS_FRAME_STOP                 0x16

#define MBUS_CONTROL_MASK_SND_NKE       0x40
#define MBUS_CONTROL_MASK_DIR_M2S       0x40
#define MBUS_ADDRESS_NETWORK_LAYER      0xFE

#define MBUS_ACK                        0xE5 (229)

// -------------------------------------------------------------------------------------
// C_field: Control Field, Function Field
// FCB: Frame count bit
// FCV: Frame count bit valid
// ADC: Access demand (slave wants to transmit class 1 data)
// DFC: Data flow control = 1 (slave cannot accept further data)

// SND_NKE Initialization of Slave:   BIN:0100 0000 HEX:40           (SHORT FRAME)
// SND_UD  Send User Data to Slave:   BIN:01F1 0011 HEX:53/73        (LONG/CONTROL FRAME)
// REQ_UD2 Request for Class 2 Data:  BIN:01F1 1011 HEX:5B/7B        (SHORT FRAME)
// REQ_UD1 Request for Class 1 Data:  BIN:01F1 1010 HEX:5A/7A        (SHORT FRAME)
// RSP_UD  Data Transfer from Slave:  BIN:00AD 1000 HEX:08/18/28/38  (LONG/CONTROL FRAME)
// F: FCB-Bit, A: ACD-Bit, D: DFC-bit
// -------------------------------------------------------------------------------------

// Address 1-250, 254,255 broadcast, 0 unconfigured

// CI Field
// 51h data send
// 52h selection of slaves
// 50h appliction reset

// --------------------------------------------------------------
// MBUS CONTROL FRAME
//
// C_field: Control Field, Function Field
// --------------------------------------------------------------
int mbus_scan() {
  unsigned long timer_start = 0;
  for (byte retry=1; retry<=3; retry++) {
    for (byte address = 1; address <= 20; address++) {
      mbus_normalize(address);
      timer_start = millis();
      while (millis()-timer_start<40) {
        if (customSerial.available()) {
          byte val = customSerial.read();
          if (val==0xE5) return address;
        }
      }
    }
  }
  return -1;
}

// ---------------------------------------------------------------

void mbus_normalize(byte address) {
  mbus_short_frame(address,0x40);
}

void mbus_request_data(byte address) {
  mbus_short_frame(address,0x5b);
}

void mbus_application_reset(byte address) {
  mbus_control_frame(address,0x53,0x50);
}

void mbus_request(byte address,byte telegram) {
 
  byte data[15];
  byte i=0;
  data[i++] = MBUS_FRAME_LONG_START;
  data[i++] = 0x07;
  data[i++] = 0x07;
  data[i++] = MBUS_FRAME_LONG_START;
  
  data[i++] = 0x53;
  data[i++] = address;
  data[i++] = 0x51;

  data[i++] = 0x01;
  data[i++] = 0xFF;
  data[i++] = 0x08;
  data[i++] = telegram;

  unsigned char checksum = 0;
  for (byte c=4; c<i; c++) checksum += data[c];
  data[i++] = (byte) checksum;
  
  data[i++] = 0x16;
  data[i++] = '\0';
  
  customSerial.write((char*)data,15);
}

void mbus_set_address(byte oldaddress, byte newaddress) {
 
  byte data[13];
 
  data[0] = MBUS_FRAME_LONG_START;
  data[1] = 0x06;
  data[2] = 0x06;
  data[3] = MBUS_FRAME_LONG_START;
  
  data[4] = 0x53;
  data[5] = oldaddress;
  data[6] = 0x51;
  
  data[7] = 0x01;         // DIF [EXT0, LSB0, FN:00, DATA 1 8BIT INT]
  data[8] = 0x7A;         // VIF 0111 1010 bus address
  data[9] = newaddress;   // DATA new address
  
  data[10] = data[4]+data[5]+data[6]+data[7]+data[8]+data[9];
  data[11] = 0x16;
  data[12] = '\0';
  
  customSerial.write((char*)data,13);
}

void mbus_set_baudrate(byte address, byte baudrate) {
 
  byte data[11];
  byte i=0;
  
  data[i++] = MBUS_FRAME_LONG_START;
  data[i++] = 0x03;
  data[i++] = 0x03;
  data[i++] = MBUS_FRAME_LONG_START;
  
  data[i++] = 0x53;
  data[i++] = address;
  data[i++] = baudrate;
  
  unsigned char checksum = 0;
  for (byte c=4; c<i; c++) checksum += data[c];
  data[i++] = (byte) checksum; 
  
  data[i++] = 0x16;
  data[i++] = '\0';
  
  customSerial.write((char*)data,11);
}

void mbus_set_id(byte address) {
 
  byte data[16];
  byte i=0;
  
  data[i++] = MBUS_FRAME_LONG_START;
  data[i++] = 0x09;
  data[i++] = 0x09;
  data[i++] = MBUS_FRAME_LONG_START;
  
  data[i++] = 0x53;
  data[i++] = address;
  data[i++] = 0x51;
  
  data[i++] = 0x0C;
  data[i++] = 0x79;
  data[i++] = 0x01; //ID1
  data[i++] = 0x02; //ID2
  data[i++] = 0x03; //ID3
  data[i++] = 0x04; //ID4
    
  unsigned char checksum = 0;
  for (byte c=4; c<i; c++) checksum += data[c];
  data[i++] = (byte) checksum; 
  
  data[i++] = 0x16;
  data[i++] = '\0';
  
  customSerial.write((char*)data,16);
}

// ---------------------------------------------------------------

void mbus_short_frame(byte address, byte C_field) {
  byte data[6];

  data[0] = 0x10;
  data[1] = C_field;
  data[2] = address;
  data[3] = data[1]+data[2];
  data[4] = 0x16;
  data[5] = '\0';

  customSerial.write((char*)data,6);
}

void mbus_control_frame(byte address, byte C_field, byte CI_field)
{
  byte data[10];
  data[0] = MBUS_FRAME_LONG_START;
  data[1] = 0x03;
  data[2] = 0x03;
  data[3] = MBUS_FRAME_LONG_START;
  data[4] = C_field;
  data[5] = address;
  data[6] = CI_field;
  data[7] = data[4] + data[5] + data[6];
  data[8] = MBUS_FRAME_STOP;
  data[9] = '\0';

  customSerial.write((char*)data,10);
}
