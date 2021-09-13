# Selecting the right heat meter

There are a number of different heat meter brands and models available that are both MID and RHI (UK Renewable Heat Incentive) approved. If you are in the UK, Stockshed provide a good range of these meters: [https://stockshed.com/pages/heat-meters-stockshed-uk](https://stockshed.com/pages/heat-meters-stockshed-uk).

Most heat meters include an MBUS interface that can be used for reading the metering data. This usually includes, total heat delivered in kWh, real-time heating power in Watts, flow and return temperatures and flow rate in L/min of m3/hr. 

Using an MBUS reader connected to an emonPi, emonBase or just a Raspberry Pi, it is possible to read in this data and then store and visualise it using emoncms. Combined with electricity data, this can provide a data rich, detailed view of the performance of a heat pump.

So far, we have only tested our MBUS reader software with a subset of the following meters but they should all be compatible, if not out of the box, with a little modification of the code. Please email me at trystanlea @ openenergymonitor.org if you are having trouble reading from a particular MBUS meter.

### Diehl Metering Sharky 775

- Tested at OpenEnergyMonitor: Yes
- Class 2, MID approved
- Ultrasonic
- Not compatible with glycol (to our knowledge)
- Power supply: **Mains AC** and battery powered options. AC power is recommended for high resolution 10s data.

### Sontex Superstatic 440

- Tested at OpenEnergyMonitor: Yes
- Class 2, MID approved
- Fluid oscillator, piezo electric
- **Glycol calibration available**
- Power supply: **Mains AC** and battery powered options. AC power is recommended for high resolution 10s data.

### Axioma Qalcosonic E3

- Tested at OpenEnergyMonitor: Yes
- Class 2, MID approved
- Ultrasonic
- Not compatible with glycol (to our knowledge)
- Power supply: **Mains AC** and battery powered options. AC power is recommended for high resolution 10s data.

### Kamstrup multical 403

- Tested at OpenEnergyMonitor: No, although original 402 was tested.
- Class 2, MID approved
- Ultrasonic
- Not compatible with glycol (to our knowledge)
- Power supply: **Mains AC** and battery powered options. AC power is recommended for high resolution 10s data.

### Sontex Superstatic 749

- Tested at OpenEnergyMonitor: No
- Class 2, MID approved
- Fluid oscillator, piezo electric
- **Glycol calibration available**
- Power supply: Battery powered only, 6 or 12 year battery life options.

### Kamstrup multical 302

- Tested at OpenEnergyMonitor: No
- Class 2, MID approved
- Ultrasonic
- Not compatible with glycol (to our knowledge)
- Power supply: Battery powered only, 6 or 12 year battery life options.

---

# Heat meter pressure loss

Written by John Cantor, see [original blog post](http://johncantorheatpumps.blogspot.co.uk/2016/06/selecting-right-heat-meter.html)

This topic is **VERY IMPORTANT**

I am aware that the pressure requirements of heat meters is often overlooked - this can be detrimental to a heat pump system.

A heat pump will normally be connected to a radiator or underfloor heat-emitter circuit.  A circulation pump (circulator) is required to ‘force’ the water around this circuit.  Each part of the pipework and each components acts as a restriction to flow. The greater the restriction, the bigger the circulation pump needed.  Indeed, many systems have 2 or more circulators, whilst it should be possible to only have one.

Heat meters  (reading heat in Kw’s) are often required to calculate RHI payments. The main component of a heat meter is a device fitted in a pipe to measure the liquid flowrate through the system. This device will cause a restriction to flow. However, it would be ridiculous for our heat-measuring device to impair energy-efficiency by either causing an inadequate liquid flow-rate, or by increasing circulation pump energy demand.

Let us get a ‘feel’ of the pressures involved. Metres head (height) of water is a good understandable unit of pressure measurement, but several other units are used;

    1 bar pressure (about 1 atmosphere)  = 10.2m head of water = 1,000mB = 100kPa

Most common circulation pumps can produce a maximum of 5 or 6 metres water head  (0.5 to 0.6 bar), however, this is at the pump’s low flow-rate.  For energy-efficient operation, we should operate them at mid-pressure range, ideally no more than a pressure of 2.5m head of water to work against.
Some heat pumps have one circulator to circulate through the heat pump and the emitters.  If the heat pump requires say 1m water head for its required flow-rate, and the emitter circuit requires a further 1m head, then the circulation pump needs to exert a total pressure of 2m head. There is little pressure left  to overcome restrictions for a the heat meter, so the heat meter should not be too restrictive.    Even if your system has 2 circulators, the head meter should not cause a significant restriction so you must SELECT THE CORRECT METER.

At their rated flow-rate, some  heat meters will require high pressure for circulation.   Some cheaper options can require up to 2.5m head pressure. This is a lot.   However, if a larger heat meter is chosen, its pressure drop can be very low since, for example,  if you halve the flow-rate, you quarter the pressure requirements.   Modern meters can retain high accuracy even at very low flow rates.
E.g. if your system flow is 1 cubic m / hour, your best low-pressure option might be to fit a 2.5 cubic m/hr device.

The example chart  below show how the pressure drop can be very low if the meter is used at a flow-rate well below its maximum.   Note also that some meters are inherently more ‘restrictive’ than others.   The colours are my own opinion, and relate to adding a heat meter to a heat pump system.

![heat_meter_pressure_loss.png](images/heat_meter_pressure_loss.png)

The Vortex Flow Sensors have exactly the same issue, and there are fewer pressure ranges to choose from.  However, in the region of 25% of their rated capacity, the pressure drop is usually nice and low.
The message here is – Don’t fit a meter without assessing the pressure drop at your expected flow rate.  Don’t necessarily trust the meter supplier since they don’t all understand the energy loss implications related to heat pumps, and may want to sell you the cheapest option.    It could be perfectly acceptable using a high/medium pressure drop heat meter  on a biomass district heating system feeding radiators, but for heat pump applications, we could be running into trouble unless the pressure drop is low.

Oddly, I see little mention of pressure drop on many heat meter data sheets.
