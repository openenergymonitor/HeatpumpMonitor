# Heatpump Monitor

This is the technical github repository for the open hardware and software heat-pump performance monitor, part of a wider heat-pump monitoring project by OpenEnergyMonitor.org and John Cantor of heatpumps.co.uk.

This is a web-connected open source hardware heat-pump monitor that can measure heat-pump electrical input, heat output and system temperatures. Alongside open source web software for heat-pump data visualisation, analysis and sharing of open heat-pump data.

![Heatpump monitor](images/topgraphic.jpg)

## Content

- Monitoring a heatpump
- System setup
- Hardware
- Firmware
- Web Dashboard

## Rationale

Heat-pumps are a key component of zero carbon energy systems making it possible to provide heating efficiently from renewable energy, they are featured in many zero carbon energy scenarios such as ZeroCarbonBritain developed by the Centre for Alternative Technology and are also highlighted in David MacKay's book Sustainable Energy without the hot air.

By monitoring a heat-pump its possible to see how well it is working, diagnose problems and get a better understanding of how a key potentially zero carbon heating solution works, data gathered and shared from well performing systems could help de-mystify heat-pump operation and help improve performance by diagnosing any problems early.

## Blogs

- [Heat pump Monitoring](https://blog.openenergymonitor.org/2015/12/heat-pump-monitoring)
- [Heatpumps in the ZeroCarbonBritain model by the Centre for Alternative Technology.](https://blog.openenergymonitor.org/2015/12/heatpumps-in-zerocarbonbritain-model-by)
- [Heat pump Testing: Initial results](https://blog.openenergymonitor.org/2016/02/heat-pump-testing-initial-results)

## [Installation and setup](installation.md)

## Hardware

![Heatpump monitor](images/HPgraphic.png)

**Main Features:**

- MBUS reader for kamstrup heat meters
- CT current and ACAC voltage based electricity monitoring 
- Pulse counting or IRDA Elster meter reader
- Analog inputs for Vortex Flow Sensor (VFS) option (e.g Grundfos, Sika) 
- 4x individually broken out DS18B20 temperature sensor connections 
- Arduino ATmega328 core 
- Connectivity options: ESP-12 WIFI, RaspberryPI header, RFM69

## Firmware

### Arduino Firmware

- [Arduino firmware that runs on the atmega328](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Firmware/Arduino/HeatpumpMonitor_V1)

To upload the Arduino firmware follow the guide [setting up the Arduino programming environment](http://openenergymonitor.org/emon/buildingblocks/setting-up-the-arduino-environment)

### ESP WIFI Firmware

 - [EmonESP Firmware for the ESP module](https://github.com/openenergymonitor/EmonESP)

Uses ESP8688 Arduino tool kit, see installation and upload notes on the EmonESP github.

## Web dashboard

Connect to `ESP` WiFi AP then browse to:

http://192.168.4.1

## Hardware V2

**New in V2:**

- Footprint for small solid state relay for volts free heating system control - not tested yet.
- Status LED

**Fixed in V2:**

- VFS requires 5V power supply, current terminals break out 3.3V, next revision needs 5V routed instead.
- Increase the thickness of the 3.3V supply lines to the ESP12 Wifi. There is now an additional 100nf cap across esp supply too
- RaspberryPi model B Ethernet port position adjusted
- Pulse counting dropdown resistor footprint on the board
- CT3 can now be selected seperatly from analog 3

BOM and example prototype costing: [BOM&Costing_oneoff_prototype.md](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Hardware/v2/BOM&Costing_oneoff_prototype.md)

## Known Issues

- There is still an issue with the ESP Wifi upload reset

## Further development

**Short term**
- Develop simpler heatpump dashboard
- Test and document use of SIKA flow meters
- Establish reliability of WIFI Connectivity
- Develop packaging for IRDA infra-red sensor
- Convert to SMT

**Longer term**
- Battery backed memory for kWh values
- Real time clock
- SD card, or on Pi Zero
- Higher capacity microcontroller
- Modbus or other electricity meter reader
- Wiring up can currently be a little fiddly, a custom enclosure design could make installation and debugging easier.

## Licence

This project is open source software and hardware.

The Arduino firmware is available under the GPL v3 Licence. 

The Web Dashboard is available under the GNU Affero General Public License and is based on emoncms.

The hardware design (schematics, board files and gerber) files are licensed under the [Creative Commons Attribution-ShareAlike 3.0 Unported License](http://creativecommons.org/licenses/by-sa/3.0/) and follow the terms of the [OSHW (Open-source hardware) Statement of Principles 1.0.](http://freedomdefined.org/OSHW)
