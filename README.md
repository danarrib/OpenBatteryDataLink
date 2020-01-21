# Open Battery Data Link
Open Battery Data Link is an open source protocol created to be the standard communication protocol for battery information for electronic devices. It's intended to be used on a sensor device connected directly to the battery cells of a battery pack, and on a battery-consuming device, to allow it to monitor the battery status.

This protocol started due a need to provide battery pack information for RC model aircraft flight controllers.

With OBDL, a sensor device can be connected to the battery balance lead, and by reading individual cells voltage, the sensor can provide more detailed information to the flight controller using a serial communication protocol.

Protocol features:
- Stackable - Supports multiple battery packs
- Supports from 1 to 16 cells per battery pack
- Battery pack's voltage and cell count
- Battery pack's average, higher and lower cell voltage
- Battery pack's instant current and used capacity measurement

Check the [Wiki](https://github.com/danarrib/OpenBatteryDataLink/wiki) for full feature list, basic DIY battery sensor schematics, OBDL protocol definition and sample implementations.

## History
The common way for a flight controller be aware of aircraft battery voltage is to wire a positive and a negative wire to a solder pad on the flight controller's board, which is wired to a simple voltage divider and to an analog input of the microcontroller. Using this method, flight controller can only be aware of the total battery pack voltage, not the individual cells voltage.

For current monitoring, flight controller needs another analog input to receive current sensor's voltage and translate it into a current measurement data (using a scale value that need to be calibrated).

So flight controller needs two analog inputs just to (poorly) monitor one battery pack. Due to the increased complexity of onboard model aircraft electronics, this analog inputs can be used to drive other sensors (like Airspeed), and free these FC inputs are a good idea.

## Integrated Flight Controllers
For now, only [InavFlight](https://github.com/iNavFlight/inav) will be integrated. But since this protocol is open, anyone can implement it on other autopilot or any electronic systems.
