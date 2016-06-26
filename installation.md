# Installation and setup

First remove the front and rear fascia from the heat pump monitor enclosure so that the terminal connections are accessible. The following image shows the function of each connector:

![heatpump_monitor_v2_connections.JPG](images/heatpump_monitor_v2_connections.JPG)

### Electricity monitoring: IRDA Elster A100C Watt Hour reader

The Heatpump Monitor supports reading from the Irda port on Elster A100C energy meters using the [OpenEnergyMonitor pulse sensor](https://shop.openenergymonitor.com/optical-utility-meter-led-pulse-sensor/). This provides a convenient way of reading the actual watt hours as recorded on the meter avoiding errors that may be caused by a multitude of sources via the other optional methods.

![IRDA Reader](images/Irdaconnection.jpg)

Selection and use of the IRDA reader over pulse counting is done in the Heatpump Monitor firmware here: [https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Firmware/Arduino/HeatpumpMonitor_V1/HeatpumpMonitor_V1.ino#L17](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/Firmware/Arduino/HeatpumpMonitor_V1/HeatpumpMonitor_V1.ino#L17)

Set ELSTER_IRDA_ENABLE to 1:

    #define ELSTER_IRDA_ENABLE 1

### Electricity monitoring: Pulse counting

The pulse counting input on the heat pump monitor is just a interrupt enabled digital input on the atmega328.

When using the pulse counting input with wired pulse output meters such as an Elster A100C or Ampy single phase meter two additional components are needed to provide reliable pulse counting. A pull down resistor is needed across the pulse input (100k) and ground and then a 100nf capacitor is needed to filter any high frequency pulses outside the expected range. The footprint for these are now on the PCB just below the terminals.

### Electricity monitoring: CT and AC-AC adapter

The heat pump monitoring board has standard OpenEnergyMonitor CT and AC-AC Adapter based electricity monitoring. The AC-AC Adapter is required for voltage signal measurement and real power calculation which gives greater measurement accuracy.

CT Burden resistor sizing: The current range measurable by the CT based measurement is dependent on both the CT sensor used and the burden resistor size on the heat pump monitoring board. In order to obtain higher measurement resolution it is recommended to size the burden resistor to match closely the maximum rated electrical power input of the heat pump.

**Calibration and Accuracy**

The CT and AC-AC power measurement circuitry was designed for indicative power measurement, better than many home energy monitors available but less accurate than billing grade class 1 or 2 electricity meters. In order to draw reliable conclusions on heat pump COP performance it is recommended to use the pulse counting input in conjunction with a class 1 or 2 electricity meter for accumulated electricity consumption measurement. This can be in parallel with the CT based measurement which has the advantage of providing higher temporal resolution power readings useful when analysing heating runs in detail.

In order to confirm the accuracy of the heat pump monitor electricity measurement note down a manual meter reading from the heat pump kwh meter when the heat pump monitor is first powered up, note down the date and time of this reading. After a measurement period of around 1 week check the accumulated electricity consumption recorded by the heat pump monitor both from the pulse counting input, the CT sensors against the consumption as read manually on the kwh meter.

### Heat metering: MBUS Kamstrup heat meter

Ensure the heat pump monitor is un-powered before connection. Connect using 2-core wire the MBUS reader terminals (top-right) with the MBUS connection of your kamstrup heat meter. The polarity does not matter but care needs to be taken not to short the two wires as this may cause damage to the MBUS interface or card.

Ensure the blue voltage converter board is fitted and positioned with the arrow pointing upwards as shown in the diagram above. The output of the voltage converter should be nearest the MBUS terminal connections.

Once connected the heat pump monitor will auto detect the Kamstrup heat meter address and pass the polled data through without further configuration.

Troubleshooting: If the kamstrup meter data does not appear straight away, wait several minutes for the reader to rescan. The voltage across the MBUS reader terminals should be close to 24V.

### Heat metering: VFS Flow meter (Sika or Grundfos)

A Vortex Flow Sensor with an analog voltage output such as the Grundfos VFS range or Sika VFS range can be connected to the VFS terminals, top-right next to the MBUS reader terminals. There are two analog inputs on the VFS terminal that map to two 10bit ADC Channels on the Atmega328. For standard configuration use A3 for flowrate measurement and A4 for temperature measurement.

Testing is ongoing to verify the potential accuracy of heat measurement based on the VFS flowrate and DS18B20 based flow and return temperature readings.

### DS18B20 Temperature sensors
