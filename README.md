# DSCKeyBus ESPHOME custom component to interface to a DSC POWERSERIES alarm system.

This is an implementation of an ESPHOME custom component to interface directly to a DSC POWERSERIES alarm system using the keybus interface and  very inexpensive ESP8266 modules (or arduino) .  Using a slightly modified DSC keybus interface library from https://github.com/taligentx/dscKeybusInterface, it provides full read and write control of the alarm system. The idea and yaml code is based on the DIY example at: https://github.com/Margriko/Paradox-ESPHome.

To install just copy both the dscalarm.yaml file and the subdirectory "dscKeybusInterface" (including all the contained dsckeybus*.cpp files) into your main esphome directory on home assistant and compile using the esphome application.

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
        value_template: "{{states('sensor.dscalarm_partition_1_status')}}"
        code_arm_required: false
        
        arm_away:
          - service: esphome.dscalarm_alarm_arm_away
                  
        arm_home:
          - service: esphome.dscalarm_alarm_arm_home
          
        arm_night:
          - service: esphome.dscalarm_alarm_arm_night
            data_template:
              code: '{{code}}' #if you didnt set it in the yaml, then send the code here
          
        disarm:
          - service: esphome.dscalarm_alarm_disarm
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
        value_template: "{{states('sensor.dscalarm_partition_1_status')}}"
        code_arm_required: false
        
        arm_away:
          - service: esphome.dscalarm_set_alarm_state
            data_template:
			  partition: 2
			  state: "A"
			  code: ""
      
        arm_home:
          - service: esphome.dscalarm_set_alarm_state
            data_template:
              partition: 2
			  state: "S"
			  code: ""

        arm_night:
          - service: esphome.dscalarm_set_alarm_state
            data_template:
              partition: 2
              state: "N"
              code: '{{code}}'  
          
        disarm:
          - service: esphome.dscalarm_set_alarm_state
            data_template:
              partition: 2
              state: "D"
              code: '{{code}}'            
```
## Services

- Basic alarm services. These services default to partition 1:

	- "alarm_disarm", Parameter: "code" (access code)
	- "alarm_arm_home" 
	- "alarm_arm_night", Parameter: "code" (access code)
	- "alarm_arm_away"
	- "alarm_trigger_panic"
	- "alarm_trigger_fire"


- Intermediate command service. Use this service if you need more versatility such as setting alarm states on any partition:

	- "set_alarm_state",  Parameters: "partition","state","code"  where partition is the partition number from 1 to 8, state is one of "D" (disarm), "A" (arm_away), "S" (arm_home), "N" (arm_night), "P" (panic) or "F" (fire) and "code" is your panel access code (can be empty for arming, panic and fire cmds )

- Generic command service. Use this service for more complex control:

	- "alarm_keypress",  Parameter: "keys" where keys can be any sequence of cmds from the list below. For example to arm partition 2  in night mode you set keys to be "/2Nxxxx" where xxxx is your access code. Alternatively you can use the * code for the same function and send /2*9xxxx.   


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

![dscinterface](https://user-images.githubusercontent.com/7193213/135171111-a1f723cf-43fc-4d4a-bdb9-61645630c7be.png)


## Alternative ground isolated version

![dscinterface-isolated](https://user-images.githubusercontent.com/7193213/135284340-49cb1df9-2527-412f-92b6-6aead3db7f7b.png)


## OTA updates
In order to make OTA updates, connection switch in frontend must be switched to OFF since the DSC library is using interrupts.

## References
* ESPHOME yaml and component structure based on code from https://github.com/Margriko/Paradox-ESPHome
* Wiring and DSC library from https://github.com/taligentx/dscKeybusInterface
* ESPHOME diy examples from https://esphome.io/guides/diy.html


