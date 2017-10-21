# Open Battery Data Link
Open Battery Data Link is an Open Source Protocol created to be a standard communication protocol for battery information for electronic devices. It's intended to be used on a sensor device connected directly to the battery cells of a battery pack, and on a battery-consuming device, so it can monitor battery status.

This protocol started due a need to provide Battery Pack information for RC Model Aircraft Flight Controllers. 

With Open Battery Data Link, a sensor device can be connected to the battery balance lead, and by reading individual cells voltage, it can provide more detailed information to the Flight Controller using a serial communication protocol.

Features:
- Stackable - Supports multiple battery packs
- Supports from 1 to 16 cells per battery pack
- Battery pack's voltage and cell count
- Battery pack's average, higher and lower cell voltage
- Battery pack's instant and total current meter

Check the [Wiki](https://github.com/danarrib/OpenBatteryDataLink/wiki) for Full feature list, Battery sensor schematics and OBDL Protocol definition and Sample Implementations.

## History
The common way for a Flight Controller be aware of aircraft battery voltage is to wire a positive and a negative wire to a solder pad on the Flight Controller's board, witch is wired to a simple voltage divider and to an analog input of the microcontroller. Using this method, Flight Controller can only be aware of the total battery pack voltage, not individual cells voltage.

For current monitoring, Flight Controller need another analog input to receive current sensor's voltage and translate it into a current measurement data (using a scale value that need to be calibrated).

So Flight Controller needs two analog inputs just to (poorly) monitor one battery pack. Due to the increased complexity of onboard model aircraft electronics, this analog inputs can be used to drive other sensors (like Airspeed), and free these FC inputs are a good idea.

## Integrated Flight Controllers
For now, only [InavFlight](https://github.com/iNavFlight/inav) will be integrated. But since this protocol is open, anyone can implement it on other autopilot systems.
