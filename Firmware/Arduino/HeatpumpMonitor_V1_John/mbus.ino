#define MBUS_FRAME_SHORT_START          0x10
#define MBUS_FRAME_STOP                 0x16
#define MBUS_CONTROL_MASK_SND_NKE       0x40
#define MBUS_CONTROL_MASK_DIR_M2S       0x40
#define MBUS_ADDRESS_NETWORK_LAYER      0xFD
// Licence: GPLv3
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

byte mbus_scan()
{
  unsigned long timer_start = 0;
  int byte_id = 0;
  for (byte address = 0; address <= 250; address++)
  {
    mbus_send_ping(address);
    byte_id = 0;
    timer_start = millis();
    while (millis()-timer_start<10)
    {
      if (customSerial->available())
      {
        byte val = customSerial->read();
        if (byte_id==0 && val==0xe5) {
            return address;
        }
        byte_id ++;
      }
    }
  }
  return 0;
}

