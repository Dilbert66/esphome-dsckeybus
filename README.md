# DSCKeyBus ESPHOME custom component to interface to a DSC POWER832 (PC5010) alarm system.

This is an implementation of an ESPHOME custom component to interface directly to a DSC POWER832 (PC5010) alarm system using the keybus interface and  very inexpensive ESP8266 modules (or arduino) .  Using a slightly modified DSC keybus interface library from https://github.com/taligentx/dscKeybusInterface, it provides full read and write control of the alarm system. The idea and yaml code is based on the DIY example at: https://github.com/Margriko/Paradox-ESPHome.

The following services are published to home assistant for use in various scripts.  Please note that you will also need to have a transistor on the green data line to give you write access to the bus as shown in the wiring diagram.

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

The returned statuses for Home Assistant are: armed_away, armed_home, armed_night, pending, disarmed,triggered and unavailable.  

Sample Home Assistant Template Alarm Control Panel configuration with simple services (defaults to partition 1):

```
alarm_control_panel:
  - platform: template
    panels:
      safe_alarm_panel:
        name: "Alarm Panel"
        value_template: "{{states('sensor.partition_1_status')}}"
        code_arm_required: false
        
        arm_away:
          - service: esphome.alarm_alarm_arm_away
                  
        arm_home:
          - service: esphome.alarm_alarm_arm_home
          
        arm_night:
          - service: esphome.alarm_alarm_arm_night
            data_template:
              code: '{{code}}'
          
        disarm:
          - service: esphome.alarm_alarm_disarm
            data_template:
              code: '{{code}}'                    

```

Alternative Home assistant configuration using specified partition with set_alarm_state service

```
alarm_control_panel:
  - platform: template
    panels:
      safe_alarm_panel:
        name: "Alarm Panel - Partition 2"
        value_template: "{{states('sensor.partition_1_status')}}"
        code_arm_required: false
        
        arm_away:
          - service: esphome.alarm_set_alarm_state
            data_template:
			  partition: 2
			  state: "A"
			  code: ""
      
        arm_home:
          - service: esphome.alarm_set_alarm_state
			data_template:
			  partition: 2
			  state: "S"
			  code: ""

        arm_night:
          - service: esphome.alarm_set_alarm_state
            data_template:
			  partition: 2
			  state: "N"
              code: '{{code}}'  
          
        disarm:
          - service: esphome.alarm_set_alarm_state
            data_template:
			  partition: 2
			  state: "D"
              code: '{{code}}'            
```
## Services

- Basic alarm services. Thes services default to partition 1.:

	"alarm_disarm" Parameter: "code" (access code)
	"alarm_arm_home" 
	"alarm_arm_night" Parameter: "code" (access code)
	"alarm_arm_away"
	"alarm_trigger_panic"
	"alarm_trigger_fire"


- Intermediate command service. Use this service if you need more versatility such as setting alarm states on any partition:

	"set_alarm_state" Parameters: "partition","state","code"  where partition is the partition number from 1 to 8, state is one of "D" (disarm), "A" (arm_away), "S" (arm_home), "N" (arm_night), "P" (panic) or "F" (fire) and "code" is your panel access code (can be empty for arming, panic and fire cmds )

- Generic command service. Use this service for more complex control:

	"alarm_keypress" Parameter: "keys" where keys can be any sequence of cmds from the list below. For example to disarm partition 2 you set keys to be "/2Dxxxx" where xxxx is your access code.   The system defaults to partition 1.


```
	  case '/': setPartition = true; validKey = false; break;
      case '0': writeKey = 0x00; break;
      case '1': writeKey = 0x05; break;
      case '2': writeKey = 0x0A; break;
      case '3': writeKey = 0x0F; break;
      case '4': writeKey = 0x11; break;
      case '5': writeKey = 0x16; break;
      case '6': writeKey = 0x1B; break;
      case '7': writeKey = 0x1C; break;
      case '8': writeKey = 0x22; break;
      case '9': writeKey = 0x27; break;
      case '*': writeKey = 0x28; writeAsterisk = true; break;
      case '#': writeKey = 0x2D; break;
      case 'F':
      case 'f': writeKey = 0x77; writeAlarm = true; break;                    // Keypad fire alarm
      case 's':
      case 'S': writeKey = 0xAF; writeArm[writePartition - 1] = true; break;  // Arm stay
      case 'w':
      case 'W': writeKey = 0xB1; writeArm[writePartition - 1] = true; break;  // Arm away
      case 'n':
      case 'N': writeKey = 0xB6; writeArm[writePartition - 1] = true; break;  // Arm with no entry delay (night arm)
      case 'A':
      case 'a': writeKey = 0xBB; writeAlarm = true; break;                    // Keypad auxiliary alarm
      case 'c':
      case 'C': writeKey = 0xBB; break;                                       // Door chime
      case 'r':
      case 'R': writeKey = 0xDA; break;                                       // Reset
      case 'P':
      case 'p': writeKey = 0xDD; writeAlarm = true; break;                    // Keypad panic alarm
      case 'x':
      case 'X': writeKey = 0xE1; break;                                       // Exit
      case '[': writeKey = 0xD5; break;                                       // Command output 1
      case ']': writeKey = 0xDA; break;                                       // Command output 2
      case '{': writeKey = 0x70; break;                                       // Command output 3
      case '}': writeKey = 0xEC; break;                                       // Command output 4
```	  

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

Write access eg. Disarm/arm/etc (option for status read only but required for alarm status control):
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


