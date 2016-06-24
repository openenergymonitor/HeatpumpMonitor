# Heatpump Monitor Build

The heatpump monitor comes as a component kit with all the resistors, capacitors, connectors, IC's and other parts required to complete the build. The full Bill of Materials can be found here:

BOM:

![heatpump_monitor_v2_components.JPG](images/heatpump_monitor_v2_components.JPG)

The following image can be used as a reference of component locations.

![heatpump_monitor_v2.JPG](images/heatpump_monitor_v2.JPG)

**Tip 1:** It is usually easier to start by soldering the lower profile components first such as the resistors and to build up to the taller components. 

**Tip 2:** Test that the ATmega328 part works first before soldering in the ESP8266 module so that any issues with the ATmega328 part can be isolated before adding the ESP WIFI part. 

Check that the power supply voltages are all as expected before inserting the ATmega328 in the 28pin holder.

**ESP8266 Supply capacitor:** An additional 10uF is required across the supply to the ESP8266 module.

### Uploading the ATmega328 Firmware



### Uploading the ESP8266 Firmware: EmonESP
