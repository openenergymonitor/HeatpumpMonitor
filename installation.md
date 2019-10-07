# Installation and setup

The following image shows the function of each connector:

![heatpump_monitor_v2_connections.JPG](images/heatpump_monitor_v2_connections.JPG)

#### Note: Heatpump Dashboard naming convention

The heatpump dashboard available on energy.emoncms.org does not have a configuration interface, it instead just looks for feeds in the emoncms.org account with the following names:

- **heatpump_elec**: Heatpump electrical power in Watts
- **heatpump_elec_kwh**: Heatpump cumulative electricity consumption in kWh 
- **heatpump_heat**: Heatpump heat output in Watts
- **heatpump_heat_kwh**: Heatpump cumulative heat output in kWh
- **heatpump_flowT**: Heatpump central heating/DHW water flow temperature.
- **heatpump_returnT**: Heatpump central heating/DHW water return temperature.
- **outside_temperature**: Outside/Ambient temperature
- **DHW_cylinderT**: Hot Water Cylinder temperature

The minimum requirement for the heatpump dashboard is a cumulative kWh feed of heatpump electricity consumption named: **heatpump_elec_kwh**.

Beyond this addition of the further feeds just extends and improves the dashboard further.

### Electricity monitoring: IRDA Elster A100C Watt Hour reader

The Heatpump Monitor supports reading from the Irda port on Elster A100C energy meters using the [OpenEnergyMonitor pulse sensor](https://shop.openenergymonitor.com/optical-utility-meter-led-pulse-sensor/). This provides a convenient way of reading the actual watt hours as recorded on the meter avoiding errors that may be caused by a multitude of sources via the other optional methods.

![IRDA Reader](images/Irdaconnection.jpg)

Selection and use of the IRDA reader over pulse counting is done in the Heatpump Monitor firmware here:

[Firmware/Arduino/HeatpumpMonitor_AutoTemp/HeatpumpMonitor_AutoTemp.ino#L19](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Firmware/Arduino/HeatpumpMonitor_AutoTemp/HeatpumpMonitor_AutoTemp.ino#L19)

Set ELSTER_IRDA_ENABLE to 1:

    #define ELSTER_IRDA_ENABLE 1
    
**When using the Elster IRDA meter reader the 100nF capacitor may need to be removed**

**Emoncms configuration**

See the [Software setup guide](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/software_setup.md) for automated setup method.

<table>
<tr><th>Input name</th><th>Input processing and feeds</th><th>Recommended feed name</th></tr>
<tr><td>PulseIRDA</td><td><b>Log to feed</b> record one standard feed with 'log to feed'.<br><b>Wh accumulator</b> record a second feed with the wh accumulator process, this process creates a feed where any data gaps are joined producing more reliable kwh graphs.</td><td>wh_accumulator: <b>heatpump_elec_kwh</b></td></tr>
</table>

### Electricity monitoring: Pulse counting

Many meters have pulse outputs, examples include: single phase and three phase electrical energy meters, Gas meters, Water meters. The pulse output may be a flashing LED or a switching relay (usually solid state) or both.

In the case of an electricity meter, a pulse output corresponds to a defined amount of energy passing through the meter (kWh/Wh). For single-phase domestic electricity meters (e.g. the Elster A100c) each pulse usually equals one Wh (1000 pulses per kWh).  With higher power meters (often three-phase), each pulse corresponds to a greater amount of energy e.g. 2, or even 10, Wh per pulse. 

The connection of the pulse counting sensor is exactly the same as in the Elster Irda meter reader connection above. The same OpenEnergyMonitor pulse sensor used for the Irda can be used for optical pulse counting when attached over the flashing red LED present on most meters. 

Alternatively wired pulse counting is also an option. For wired pulse counting two wires are needed one from the 3.3V supply terminal going to the meter and another switched pulse output coming back to the PULSE input terminal on the heatpump monitor.

When using the pulse counting input with wired pulse output meters such as an Elster A100C or Ampy single phase meter two additional components are needed to provide reliable pulse counting. A pull down resistor is needed across the pulse input (100k) and ground and then a 100nf capacitor is needed to filter any high frequency pulses outside the expected range. The footprint for these are now on the PCB just below the terminals.

For more information on pulse counting see the Building Blocks page: [Introduction to Pulse Counting](https://learn.openenergymonitor.org/electricity-monitoring/pulse-counting/introduction-to-pulse-counting)

The pulse counting input on the heatpump monitor is just a interrupt enabled digital input on the atmega328.

**Emoncms configuration**

<table>
<tr><th>Input name</th><th>Input processing and feeds</th><th>Recommended feed name</th></tr>
<tr><td>PulseIRDA</td><td><b>Log to feed</b> record one standard feed with 'log to feed'.<br><b>Wh accumulator</b> record a second feed with the wh accumulator process, this process creates a feed where any data gaps are joined producing more reliable kwh graphs.</td><td>wh_accumulator: <b>heatpump_elec_kwh</b></td></tr>
</table>

### Electricity monitoring: CT and AC-AC adapter

![CTenergymonitoring.jpg](images/CTenergymonitoring.jpg)

In addition to the elster irda meter reader and pulse counting options the heatpump monitor also supports standard OpenEnergyMonitor CT and AC-AC Adapter based electricity monitoring. The AC-AC Adapter is required for voltage signal measurement and real power calculation which gives greater measurement accuracy.

CT Burden resistor sizing: The current range measurable by the CT based measurement is dependent on both the CT sensor used and the burden resistor size on the heat pump monitoring board. In order to obtain higher measurement resolution it is recommended to size the burden resistor to match closely the maximum rated electrical power input of the heat pump.

The included burden resistor in the heatpump monitor kit is 100 Ohms, this provides a AC power measurement range of **0 - 5.4 kW**. This provides ample range for many small domestic heatpumps. A smaller burden resistor can be used instead for larger measurement range at lower resolution. See Building Blocks guide [CT sensors - interfacing with arduino](https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino)

**Calibration and Accuracy**

The CT and AC-AC power measurement circuitry was designed for indicative power measurement, better than many home energy monitors available but less accurate than billing grade class 1 or 2 electricity meters. In order to draw reliable conclusions on heat pump COP performance it is recommended to use the Elster Irda Watt hour reader or pulse counting input in conjunction with a class 1 or 2 electricity meter for accumulated electricity consumption measurement. This can be in parallel with the CT based measurement which has the advantage of providing higher temporal resolution power readings useful when analysing heating runs in detail.

In order to confirm the accuracy of the heat pump monitor electricity measurement note down a manual meter reading from the heat pump kwh meter when the heat pump monitor is first powered up, note down the date and time of this reading. After a measurement period of around 1 week check the accumulated electricity consumption recorded by the heat pump monitor both from the pulse counting input, the CT sensors against the consumption as read manually on the kwh meter.

See [Building Blocks: Sources of error in the emontx voltage and current inputs](https://learn.openenergymonitor.org/electricity-monitoring/ctac/emontx-error-sources)

**Emoncms configuration**

<table>
<tr><th>Input name</th><th>Input processing and feeds</th><th>Recommended feed name</th></tr>
<tr><td>OEMct1</td><td><b>Log to feed</b> record one standard power feed with 'log to feed'.<br><b>power to kWh</b> record a second feed with the power to kwh process, this process creates a cumulative kWh feed from the power input.</td><td>log_to_feed: <b>heatpump_elec</b><br>power_to_kwh: <b>heatpump_elec_kwh</b></td></tr>
<tr><td>OEMct2</td><td><b>Log to feed</b> record one standard power feed with 'log to feed'.<br><b>power to kWh</b> record a second feed with the power to kwh process, this process creates a cumulative kWh feed from the power input.</td><td></td></tr>
<tr><td>OEMct1Wh</td><td><b>Wh accumulator</b> use the wh accumulator process to record the cumulative watt hour data in a feed where any data gaps are joined producing more reliable kwh graphs.</td><td>heatpump_elec_kwh</td></tr>
<tr><td>OEMct2Wh</td><td><b>Wh accumulator</b> use the wh accumulator process to record the cumulative watt hour data in a feed where any data gaps are joined producing more reliable kwh graphs.</td><td></td></tr>
</table>

### Heat metering: MBUS Kamstrup heat meter

![kamstrup.jpg](images/kamstrup.jpg)

A core feature of the heatpump monitor board is the MBUS meter reader and integrated firmware for reading heat metering data from Kamstrup Multical 402 heat meters. [https://www.kamstrup.com/en-en/products-solutions/thermal-energy-meters/multical-403](https://www.kamstrup.com/en-en/products-solutions/thermal-energy-meters/multical-403)

Ensure the heat pump monitor is un-powered before connection. Connect using 2-core wire the MBUS reader terminals (top-right) with the MBUS connection of your kamstrup heat meter. The polarity does not matter but care needs to be taken not to short the two wires as this may cause damage to the MBUS interface or card.

Ensure the blue voltage converter board is fitted and positioned with the arrow pointing upwards as shown in the diagram above. The output of the voltage converter should be nearest the MBUS terminal connections.

Once connected the heat pump monitor will auto detect the Kamstrup heat meter address and pass the polled data through without further configuration.

Troubleshooting: If the kamstrup meter data does not appear straight away, wait several minutes for the reader to rescan. The voltage across the MBUS reader terminals should be close to 24V.

**Emoncms configuration**

<table>
<tr><th>Input name</th><th>Input processing and feeds</th><th>Recommended feed name</th></tr>
<tr><td>KSflowT</td><td>Flow temperature (Celcius)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td><td>heatpump_flowT</td></tr>
<tr><td>KSreturnT</td><td>Return temperature (Celcius)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td><td>heatpump_returnT</td></tr>
<tr><td>KSdeltaT</td><td>Difference between flow and return temperature (Kelvin)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td><td>heatpump_deltaT</td></tr>
<tr><td>KSflowrate</td><td>Water Flowrate (Litres/hour)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td><td></td></tr>
<tr><td>KSheat</td><td>Heatpump heat output (Watts)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.<br><b>Power to kWh</b> use the power to kwh process to generate a higher resolution kWh feed from the kamstrup power data to be used in parallel with the kWh reading from the kamstrup meter.</td><td>log_to_feed: <b>heatpump_heat</b><br>power_to_kwh: <b>heatpump_heat_kwh</b></td></tr>
<tr><td>KSkWh</td><td>Total cumulative kWh of heat measured by kamstrup (kWh)<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td><td>heatpump_heat_kwh</td></tr>
</table>

### Heat metering: VFS Flow meter (Sika or Grundfos)

![vfsflowmeter.jpg](images/vfsflowmeter.jpg)

A Vortex Flow Sensor with an analog voltage output such as the Grundfos VFS range or Sika VFS range can be connected to the VFS terminals, top-right next to the MBUS reader terminals. There are two analog inputs on the VFS terminal that map to two 10bit ADC Channels on the Atmega328. For standard configuration use A3 for flowrate measurement and A4 for temperature measurement.

There are a multitude of VFS flow sensors each catering for different flow ranges, the calibration for the VFS sensor is set in the Heatpump monitor firmware here:

[Firmware/Arduino/HeatpumpMonitor_AutoTemp/HeatpumpMonitor_AutoTemp.ino#L52](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Firmware/Arduino/HeatpumpMonitor_AutoTemp/HeatpumpMonitor_AutoTemp.ino#L52)

    // --------------------------------------------------
    // VFS config
    // --------------------------------------------------
    double VFS_maxflow          = 40.0;  // Litres/minute
    double VFS_maxflow_voltage  = 3.5;   // Volts
    double VFS_zeroflow_voltage = 0.25;  // Volts
    double VFS_maxtemp          = 100;   // Celcius
    double VFS_maxtemp_voltage  = 3.5;   // Volts
    double VFS_zerotemp_voltage = 0.5; // Volts
    
The calibration values to use here can be found the VFS sensor datasheet.

Testing is ongoing to verify the potential accuracy of heat measurement based on the VFS flowrate and DS18B20 based flow and return temperature readings.

**Emoncms configuration**

<table>
<tr><th>Input name</th><th>Input processing and feeds</th><th>Recommended feed name</th></tr>
<tr><td>VFSflowT</td><td>Flow temperature measured by temperature sensor in VFS flow meter.<br><b>Log to feed</b> record to PHPFina feed at 10s interval</td><td>heatpump_flowT</td></tr>
<tr><td>VFSflowrate</td><td>Flow rate in Litres/hour.<br><b>Log to feed</b> record to PHPFina feed at 10s interval.<br><b>Log to feed</b> record to PHPFina feed at 10s interval.</td></tr>
<tr><td>VFSheat</td><td>Heat output measured in Watts by VFS<br><b>Log to feed</b> record power to PHPFina feed at 10s interval.<br><b>Power to kWh</b> use the power to kwh process to calculate cumulative kWh feed</td><td>log_to_feed: <b>heatpump_heat</b><br>power_to_kwh: <b>heatpump_heat_kwh</b></td></tr>
</table>


### DS18B20 Temperature sensors

**Emoncms configuration**

Using the latest HeatpumpMonitor_AutoTemp firmware, DS18B20 temperature sensors are detected automatically and sent with a key/name that includes a unqiue ID so that removing a fautly sensor does not mix up the order. The naming is of the form: T[ID], T682.

These temperature inputs can then be named within emoncms to a more useful descriptive name.

## Power up

With the sensors connected as above and ATmega firmware configured as needed, the final step is to power up the unit and configure the WIFI.

**ATmega to ESP8266 jumper link** For data to be transfered from the ATmega microcontroller across to the ESP8266 module the jumper link labeled TX between both 6way UART headers needs to be linked across. This creates a serial link between both units. This jumper needs to be removed before programming either the ESP8266 module or the ATmega328.

### EmonESP Config

On first power up the ESP WIFI module will create a WIFI access point. Connect to this directly and once connected navigate to ip address 192.168.4.1 in your browser.

The EmonESP interface which looks much like this should appear:

![EmonESP](images/EmonESP.png)

Use this interface to scan and connect to the WIFI network you wish to connect to. Once connected the IP address of the heatpump monitor will be shown. Navigate to this IP address and switch over from the WIFI access point to your local network. 

Next enter you emoncms.org account apikey. If data is appearing in the Latest data values pane and the emoncms apikey is set you should now see a set of inputs appear on your emoncms.org inputs page.

### Emoncms Inputs

The input list will look much like this but without the configured feeds. Use the emoncms configuration details above to configure emoncms to record the heatpump monitor data.

![emoncms_inputs.png](images/emoncms_inputs.png)

### Emoncms Feeds

![emoncms_feeds.png](images/emoncms_feeds.png)

### Emoncms Heatpump Monitor dashboard

![emoncms_hpmon_app_1.png](images/emoncms_hpmon_app_1.png)

![emoncms_hpmon_app_2.png](images/emoncms_hpmon_app_2.png)



