# Energy Monitor Build

A cut down version of the heatpump monitor can be used as a WIFI energy monitor without the additional components used for the MBUS reader, RFM69 and other connectors.

BOM:

![energymonitor_v2_components.JPG](images/energymonitor_v2_components.JPG)

Component values including resistor resistance and capacitor capacitance are printed on the PCB. See resistor color code graphic here to match the resistors [images/resistor-color-chart.jpg](images/resistor-color-chart.jpg).

Several of the resistors only have a reference name printed:

    VDT: Voltage Divider Top (for AC-AC Voltage sensor) value: 470k (yellow, purple, yellow)
    VDB: Voltage Divider Bottom (for AC-AC Voltage sensor) value: 470k (yellow, purple, yellow)
    VDT1: Voltage Divider Top 1 (for CT sensor 1) value: 470k (yellow, purple, yellow)
    VDB1: Voltage Divider Bottom 1 (for CT sensor 1) value: 470k (yellow, purple, yellow)
    VDT2: Voltage Divider Top 2 (for CT sensor 2) value: 470k (yellow, purple, yellow)
    VDB2: Voltage Divider Bottom 2 (for CT sensor 2) value: 470k (yellow, purple, yellow)
    VDT3: Voltage Divider Top 3 (for CT sensor 3) value: 470k (yellow, purple, yellow)
    VDB3: Voltage Divider Bottom 3 (for CT sensor 3) value: 470k (yellow, purple, yellow)

CT Burden resistors:

    BUR1: Burden resistor 1 (for CT sensor 1) value: 100R
    BUR2: Burden resistor 2 (for CT sensor 2) value: 100R
    BUR3: Burden resistor 3 (for CT sensor 3) value: 100R
    
Capacitors: C11, C13, C12, C19 all 10uF

The following image can also be used as a reference of component locations. 
    
![energymonitor_v2.JPG](images/energymonitor_v2.JPG)

**Tip 1:** It is usually easier to start by soldering the lower profile components first such as the resistors and to build up to the taller components. 

**Tip 2:** Test that the ATmega328 part works first before soldering in the ESP8266 module so that any issues with the ATmega328 part can be isolated before adding the ESP WIFI part. 

Check that the power supply voltages are all as expected before inserting the ATmega328 in the 28pin holder.

**ESP8266 Supply capacitor:** An additional 10uF is required across the supply to the ESP8266 module. The ground is top-left and 3.3v top-right.

### Uploading the ATmega328 Firmware

The default heatpump monitor firmware can be found here: [https://github.com/openenergymonitor/HeatpumpMonitor/tree/master/Firmware/Arduino/HeatpumpMonitor_V1](https://github.com/openenergymonitor/HeatpumpMonitor/tree/master/Firmware/Arduino/HeatpumpMonitor_V1)

Use a USB to UART programmer connected to the 6-way UART header closest to the ATmega328 to upload this firmware using the Arduino IDE. There's a guide on uploading Arduino firmware here: [http://openenergymonitor.org/emon/buildingblocks/setting-up-the-arduino-environment](http://openenergymonitor.org/emon/buildingblocks/setting-up-the-arduino-environment)

### Uploading the ESP8266 Firmware: EmonESP

The EmonESP ESP8266 Firmware and upload guide can be found here: [https://github.com/openenergymonitor/EmonESP](https://github.com/openenergymonitor/EmonESP)

**GPIO0 Jumper:** Place a jumper across the GPIO0 header just left of the ESP8266 module to place the ESP8266 module in bootloader mode. Power cycle the board to reset the ESP8266 module. Once the firmware is uploaded remove the GPIO0 jumper so that the ESP8266 starts up without going into bootloader mode at the next power cycle.
