# DSCKeyBus ESPHOME custom component to interface to a DSC POWER832 (PC5010) alarm system.

This is an implementation of an ESPHOME custom component to interface directly to a DSC POWER832 (PC5010) alarm system using the keybus interface and  very inexpensive ESP8266 modules (or arduino) .  Using a slightly modified DSC keybus interface library from https://github.com/taligentx/dscKeybusInterface, it provides full read and write control of the alarm system. The idea and yaml code is based on the DIY example at: https://github.com/Margriko/Paradox-ESPHome.

The following services are published to home assistant for use in various scripts:

	alarm_disarm: Disarms the alarm with the user code provided, or the code specified in the configuration.
	alarm_arm_home: Arms the alarm in home mode.
	alarm_arm_away: Arms the alarm in away mode.
	alarm_arm_night: Arms the alarm in night mode (no entry delay).
	alarm_trigger_panic: Trigger a panic alarm.
    alarm_trigger_fire: Trigger a fire alarm.
	alarm_keypress: Sends a string of characters to the alarm system. 

These are similar services to those provided by the Envisalink Home Assistant integration (which requires a more expensive Envisalink interface board to be installed in your alarm system panel). See: https://www.home-assistant.io/integrations/envisalink/

## Example in Home Assistant
![Image of HASS example](https://github.com/Dilbert66/esphome-dsckeybus/blob/master/dsckeybusinterface.png)

## Wiring

```
DSC Aux(+) ---+--- Arduino Vin pin
              |
              +--- esp8266 NodeMCU Vin pin
              |
              +--- 5v voltage regulator --- esp8266 Wemos D1 Mini 5v pin

DSC Aux(-) --- Arduino/esp8266 Ground

                                   +--- dscClockPin (Arduino Uno: 2,3 / esp8266: D1,D2,D8)
DSC Yellow --- 15k ohm resistor ---|
                                   +--- 10k ohm resistor --- Ground

                                   +--- dscReadPin (Arduino Uno: 2-12 / esp8266: D1,D2,D8)
DSC Green ---- 15k ohm resistor ---|
                                   +--- 10k ohm resistor --- Ground

Virtual keypad (optional):
DSC Green ---- NPN collector --\
                                |-- NPN base --- 1k ohm resistor --- dscWritePin (Arduino Uno: 2-12 / esp8266: D1,D2,D8)
      Ground --- NPN emitter --/
```

## Wiring Notes
### See DSC interface project for more info: https://github.com/taligentx/dscKeybusInterface

* The DSC Keybus operates at ~12.6v, a pair of resistors per data line will bring this down to an appropriate voltage for both Arduino and esp8266.
* Arduino: connect the DSC Yellow (Clock) line to a [hardware interrupt pin](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/) - for the Uno, these are pins 2 and 3.  The DSC Green (Data) line can be connected to any of the remaining digital pins 2-12.
* esp8266: connect the DSC lines to GPIO pins that are normally low to avoid putting spurious data on the Keybus: D1 (GPIO5), D2 (GPIO4) and D8 (GPIO15).
* Write access to the Keybus requires an NPN transistor and a resistor.  Most small signal NPN transistors should be suitable.
* If using an external power supply (such as the usb connector), ensure you still connect the ground to the DSC panel. 

## OTA updates
In order to make OTA updates, connection switch in frontend must be switched to OFF since the DSC library is using interrupts.

## References
* ESPHOME yaml and component structure based on code from https://github.com/Margriko/Paradox-ESPHome
* Wiring and DSC library from https://github.com/taligentx/dscKeybusInterface
* ESPHOME diy examples from https://esphome.io/guides/diy.html

