# The Original Through-hole Heatpump Monitor

This directory contains all the documentation, hardware schematics, board files and firmware for the original OpenEnergyMonitor through-hole heat pump monitor, developed between 2015-2017.

This is a web-connected open source hardware heat-pump monitor that can measure heat-pump electrical input with CT sensors, heat output with an MBUS reader or voltage output flow sensor and system temperatures using DS18B20 temperature sensors. The design is based on both the Ardunio and ESP8266 WiFi platforms.

![Heatpump monitor](images/topgraphic.jpg)

Blog post written late 2015 discussing this design: [Heat pump Monitoring](https://blog.openenergymonitor.org/2015/12/heat-pump-monitoring)

## Available in the OpenEnergyMonitor Shop

The last set of through-hole kits are still available in our shop as of the time of writting (late 2021).

[https://shop.openenergymonitor.com/esp8266-wifi-heatpump-monitor](https://shop.openenergymonitor.com/esp8266-wifi-heatpump-monitor)

## Overview

<table>
<tr><td>
<img style="width:40%" src="images/HPgraphic.png">
</td><td>
<b>Main Features:</b><br>
<ul>
<li>CT current and ACAC voltage based electricity monitoring</li>
<li>Pulse counting or IRDA Elster meter reader</li>
<li>Analog inputs for Vortex Flow Sensor (VFS) option (e.g Sika, Grundfos)</li>
<li>MBUS reader for kamstrup heat meters (402, 403)</li>
<li>4x individually broken out DS18B20 temperature sensor connections</li>
<li>Arduino ATmega328 core</li>
<li>Connectivity options: ESP-12 WIFI, RaspberryPI header, RFM69</li>
</ul>
</td>
</tr>
</table>

## Build Guides

There are two build options for the heatpump monitor board, the first is the full heatpump monitor build with components for the MBUS heat meter reader, DS18B20 temperature sensing plus other interfaces. The second is a cut-down WIFI electricity monitor only build where the additional components for heat metering are not soldered in.

- [Heatpump monitor kit build guide](heatpumpmonitor_build.md)
- [Cut down energy monitor kit build guide](energymonitor_build.md)

**Quick reference**

- [ATmega328 Arduino Firmware: HeatpumpMonitor_AutoTemp](https://github.com/openenergymonitor/HeatpumpMonitor/blob/master/HeatpumpMonitorTH/Firmware/Arduino/HeatpumpMonitor_AutoTemp)
- [ESP8266 EmonESP WIFI Firmware](https://github.com/openenergymonitor/EmonESP)
- [Hardware Schematic and Board files v4](https://github.com/openenergymonitor/HeatpumpMonitor/tree/master/HeatpumpMonitorTH/Hardware/v4)

## Installation and setup

The following guide details how to setup the heatpump monitor including interfacting with an Elster A100C Irda port and connecting up to a heat meter.

- [Selecting the right heat meter](../selectingheatmeter.md)
- [Installation and setup](installation.md)
- [Blog: Temperature sensors for monitoring heat pumps](http://johncantorheatpumps.blogspot.co.uk/2015/06/temperature-sensing-with.html)
- [Firmware Upload / Update](firmware-upload.md)

## Open Hardware ++

A case study in open hardware development

- Exporting gerbers from EAGLE, getting a PCB made with a PCB manufacturer such as Ragworm
- [Heatpump Monitor and cut down energy monitor kit costing](costingexample.md)
