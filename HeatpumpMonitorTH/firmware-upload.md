## ESP Firmware Upload 

HP monitors runs [EmonESP WiFi Emoncms link firmware](https://github.com/openenergymonitor/EmonESP)

- Ensure Tx jumper is open (not connected)
- Close GPIO0 jumper (required to enter bootloder mode)
- Power cycle the board by un-plugging then plugging in mini-usb 5V DC power 
- Upload via UART closest to ESP @ 115200 baud 
- See [EmonESP readme](https://github.com/openenergymonitor/EmonESP) for PlatformIO or ArduinoIDE instructions
- Remove GPIO0 and power cycle

## ATmega328 Fimrware Upload 

- Ensure Rx and Tx jumpers are open (removed)
- Upload via UART closest to ATmega328 
