avrdude -v -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 115200 -U flash:w:HeatpumpMonitor_AutoTemp_MultiPage.ino.hex
