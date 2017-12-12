// Licence: GPL v3
#define MBUS_FRAME_SHORT_START          0x10
#define MBUS_FRAME_STOP                 0x16
#define MBUS_CONTROL_MASK_SND_NKE       0x40
#define MBUS_CONTROL_MASK_DIR_M2S       0x40
#define MBUS_ADDRESS_NETWORK_LAYER      0xFD

void mbus_send_ping(byte address) {
  byte data[5];
  
  data[0] = MBUS_FRAME_SHORT_START;
  data[1] = MBUS_CONTROL_MASK_SND_NKE;
  data[2] = address;
  data[3] = data[1] + data[2]; // checksum
  data[4] = MBUS_FRAME_STOP;

  customSerial->write((char*)data);
}

void mbus_request_data(byte address) {
  byte data[5];
 
  data[0] = MBUS_FRAME_SHORT_START;
  data[1] = 0x5b;
  data[2] = address;
  data[3] = data[1]+data[2];
  data[4] = MBUS_FRAME_STOP;
  data[5] = '\0';

  customSerial->write((char*)data);
}

void mbus_normalize() {
  byte data[5];
  
  data[0] = MBUS_FRAME_SHORT_START;
  data[1] = MBUS_CONTROL_MASK_SND_NKE;
  data[2] = MBUS_ADDRESS_NETWORK_LAYER;
  data[3] = data[1] + data[2]; // checksum
  data[4] = MBUS_FRAME_STOP;

  customSerial->write((char*)data);
}

int mbus_scan()
{
  wdt_reset();
  unsigned long timer_start = 0;
  int byte_id = 0;
  for (int address = 0; address < 256; address++)
  {
    mbus_request_data(address);
    byte_id = 0;
    timer_start = millis();
    while (millis()-timer_start<50)
    {
      if (customSerial->available())
      {
        byte val = customSerial->read();
        if (val) {
            return address;
        }
        byte_id ++;
      }
    }
    if (address==50 || address==100 || address==150 || address==200) {
      if (DEBUG) Serial.print(".");
      wdt_reset();
    }
  }

  wdt_reset();
  return -1;
}

