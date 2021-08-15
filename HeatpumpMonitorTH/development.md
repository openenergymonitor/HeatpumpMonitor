# Development

The heatpump monitor hardware is now in its fourth revision. 

Version 4 included a small revision adding a SMT push switch footprint to the board.

Version 3 changed the voltage regulator to a high current option.

Version 2 replaced the EmonGLCD footprint with a solid state relay and fixed a couple of issues with the first board. 

There are several aspects that would be good to develop further (discussed below) but the heatpump monitor in its present form is a good starting point providing a relatively low cost web-connected WIFI heatpump monitor development board that can interface with Elster A100C Irda electricity monitoring, Kamstrup MBUS heat meters and SIKA flow meters.

One of the main questions for the next stage of the heatpump monitor is whether the core features of the heatpump monitor can be integrated in a redesign of the pre-assembled SMT EmonTx v3 including the ESP8266 WIFI module. It would make more sense from a manufacturing perspective to be able to focus around a single unit. 

Another key development will be to increase the accuracy of the CT based electricity monitoring. Accurate electricity energy measurement is currently achieved by interfacing with an Elster A100C Irda port to read the cumulative watt hour reading.

**V2:**

- New: Footprint for small solid state relay for volts free heating system control - not tested yet.
- New: Status LED
- VFS requires 5V power supply, current terminals break out 3.3V, V2 revision has 5V routed instead.
- Increased thickness of the 3.3V supply lines to the ESP12 Wifi. There is now an additional 100nf cap across esp supply too
- RaspberryPi model B Ethernet port position adjusted
- Pulse counting dropdown resistor footprint on the board
- CT3 can now be selected seperatly from analog 3

**Known Issues**

- There is still an issue with the ESP Wifi upload reset

**Further development:**

- Test and document use of SIKA flow meters
- Establish reliability of WIFI Connectivity

- Higher accuracy CT based electricity monitoring either with special purpose IC or high resolution ADC.
- Battery backed memory for kWh values
- Real time clock
- SD card for local data logging
- Modbus or other electricity meter reader
- Wiring up can currently be a little fiddly, a custom enclosure design could make installation and debugging easier.

