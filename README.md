# DSCKeyBus ESPHOME custom component to interface to a DSC POWERSERIES alarm system.

This is an implementation of an ESPHOME custom component to interface directly to a DSC POWERSERIES alarm system using the keybus interface and  very inexpensive ESP8266/ESP32 modules . NOTE: I strongly suggest using an ESP32 for it's increased capabilities. 

Using a heavily modified DSC keybus interface library from https://github.com/taligentx/dscKeybusInterface, it provides full read and write control of the alarm system. The idea and yaml code is based on the DIY example at: https://github.com/Margriko/Paradox-ESPHome.

To install just copy both the dscalarm.yaml file and the subdirectory "dscKeybusInterface" (including all the contained dsckeybus*.cpp files) into your main esphome directory on home assistant and compile using the esphome application.

The following services are published to home assistant for use in various scripts.  Please note that you will also need to have a transistor on the green data line to give you write access to the bus as shown in the wiring diagram.

	alarm_disarm: Disarms the alarm with the user code provided, or the code specified in the configuration.
	alarm_arm_home: Arms the alarm in home mode.
	alarm_arm_away: Arms the alarm in away mode.
	alarm_arm_night: Arms the alarm in night mode (no entry delay).
	alarm_trigger_panic: Trigger a panic alarm.
        alarm_trigger_fire: Trigger a fire alarm.
	alarm_keypress_partition: Sends a string of characters to the alarm system to the selected partition
        set_zone_fault: Triggers a open/close on one of the zone emulator virtual zones
	set_panel_time: Set the time on your panel. For the esp32 on. Automatically syncs the time with an nntp server.
	set_panel_time_manual: Set the time on your panel directly.
    
    
These are similar services to those provided by the Envisalink Home Assistant integration (which requires a more expensive Envisalink interface board to be installed in your alarm system panel). See: https://www.home-assistant.io/integrations/envisalink/

A new feature of the firmware is the ability to emulate multiple PC5108 zone adapaters to give you up to 64 zones if your system supports it. Example configuration to add emulation of zones 17-24 in the dscalarm.yaml file.  

Example config: 
```
  #zone expander addresses:
  # 9  - zones 9-16
  # 10 - zones 17-24
  # 11 - zones 25-32
  # 12 - zones 33-40 (for systems with 64 zone support)
  # 13 - zones 41-48 (for systems with 64 zone support)
  # 14 - zones 49-56 (for systems with 64 zone support)
  # 16 - zones 57-64 (for systems with 64 zone support)  
  expanderAddr1: "10" # 1st zone expander emulator address to use . Set to 0 to disable. 
  expanderAddr2: "0" # 2nd expander emulator address to use . Set to 0 to disable. 
```
A new service is also made available in home assistant called  esphome.<yourdscsystemname>_set_zone_fault with attributes zone and fault: 
zone is 2 digit zone number and fault is 0 or 1 where 1 sets a zone as open and 0 closes it. By calling this service from a home assistant function, you can enable any event, sensor, etc to trigger any one of your emulated zones in your alarm system.

Relay module support is also added.   This will give your panel the ability to trigger an event on any PGM channel output.  See the yaml file for an example configuration.  An example output for pgm 1 will show in home assistant as <systemname> PGM 1. You can add more fields in the yaml config file.

This version now also adds the ability to display the low battery warning status of any wireless zones.  These will be showng in the "zone status" field of the yaml.   Also in that field, you will see the alarm status, open and bypass statuses of any zones
The display format for a low battery is BL:zz and alarm status will be shown as AL:zz  where zz is the zone number.  Bypass is BY:xx and open is OP:xx.

I've also added a custom alarm panel card with new  functions from the library that provides similar functions as available from an LCD keypad.  This includes the display menus and the ability to fully program your system using full text display of all fields.
	
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
    - "set_zone_fault", Parameters: "zone" , "fault" eg zone: 17, fault: 1 (or true) 


- Intermediate command service. Use this service if you need more versatility such as setting alarm states on any partition:

	- "set_alarm_state",  Parameters: "partition","state","code"  where partition is the partition number from 1 to 8, state is one of "D" (disarm), "A" (arm_away), "S" (arm_home), "N" (arm_night), "P" (panic) or "F" (fire) and "code" is your panel access code (can be empty for arming, panic and fire cmds )

- Generic command service. Use this service for more complex control:

	- "alarm_keypress",  Parameter: "keys" where keys can be any sequence of cmds from the list below. For example to arm partition 2  in night mode you set keys to be "/2Nxxxx" where xxxx is your access code. Alternatively you can use the * code for the same function and send /2*9xxxx.   


```
	  case '/': setPartition = true; validKey = false; break;
      case '0':
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

## Custom Alarm Panel Card
I've added a sample lovelace alarm-panel card copied from the repository at https://github.com/GalaxyGateway/HA-Cards. I've customized it to work with this ESP library's services.  To configure the card, just place the main script alarm-panel-card.js and associated *.mp3 sound files into the /config/www directory of your homeassistant installation and add a new resource in your lovelace configuration pointing to /local/alarm-panel-card.js. You can then configure the card as shown below. Just substitute your service name to your application. This is an example for using it in a multi partition environment where you can have separate distinct cards for each partition just by changing the services used and partition numbers.
### Panel Card example
	
![dscpanel](https://user-images.githubusercontent.com/7193213/169652187-bb5e88ce-ec94-48e7-b674-6346cd7520c7.png)

### Panel card example lovelace configuration
```
type: custom:alarm-keypad-card
title: DSCALARM_ESPHOME
unique_id: dsc1
disp_line1: sensor.dscalarm_line1
disp_line2: sensor.dscalarm_line2
service_type: esphome
service: dscalarm_alarm_keypress_partition
status_A: READY
status_B: ARMED
status_C: TROUBLE
status_D: AC
status_E: ''
status_F: ''
status_G: ''
status_H: ''
sensor_A: binary_sensor.dscalarm_partition_1_ready
sensor_B: binary_sensor.dscalarm_partition_1_armed
sensor_C: binary_sensor.dscalarm_trouble_status
sensor_D: binary_sensor.dscalarm_ac_status
sensor_E: ''
sensor_F: ''
sensor_G: ''
sensor_H: ''
button_A: STAY
button_B: AWAY
button_C: NIGHT
button_D:
button_E:
button_F: <
button_G: '>'
button_H: 
cmd_A:
  keys: S
  partition: 1
cmd_B:
  keys: W
  partition: 1  
cmd_C:
  keys: 'N'
  partition: 1
cmd_D:
  keys:
  partition: 1    
cmd_E:
  keys:
  partition: 1    
cmd_H:
  keys:
  partition: 1  
cmd_F:
  keys: <
  partition: 1
cmd_G:
  keys: '>'
  partition: 1
key_0:
  keys: '0'
  partition: 1
key_1:
  keys: '1'
  partition: 1
key_2:
  keys: '2'
  partition: 1
key_3:
  keys: '3'
  partition: 1
key_4:
  keys: '4'
  partition: 1
key_5:
  keys: '5'
  partition: 1
key_6:
  keys: '6'
  partition: 1
key_7:
  keys: '7'
  partition: 1
key_8:
  keys: '8'
  partition: 1
key_9:
  keys: '9'
  partition: 1
key_star:
  keys: '*'
  partition: 1
key_pound:
  keys: '#'
  partition: 1
text_1: BYPASS
text_2: SERV
text_3: ALARMS
text_4: CHIME
text_5: CODES
text_6: FUNC
text_7: OUTP
text_8: PROG
text_9: NIGHT
text_star: SELECT
text_pound: ENTER
text_0: ''  
beep: sensor.dscalarm_beeps
scale: 1
view_pad: true
view_display: true
view_status: true
view_status_2: false
view_bottom: true
	
```	
	
	
	
## Wiring
### Isolated version
![dscinterface-isolated](https://user-images.githubusercontent.com/7193213/132961092-f89af5bd-0ca2-4330-8a35-38195d87f4b8.png)
### Non-isolated simple version	
![Image of Wiring schematic](https://github.com/Dilbert66/esphome-dsckeybus/blob/master/dscinterface.png)

## Wiring Notes
### See DSC interface project for more info: https://github.com/taligentx/dscKeybusInterface

* esp8266: connect the DSC lines to GPIO pins that are normally low to avoid putting spurious data on the Keybus: GPIO5, GPIO4 and GPIO15.
* esp32: connect the DSC lines to GPIO pins that are normally low to avoid putting spurious data on the Keybus: GPIO22, GPIO21 and GPIO18.
  Note: You can use a Wemos D1 Mini PRO (ESP32) with the same foot print as an original esp8266 based d1 mini. In that case use pins GPIO22,GPIO21 and GPIO5
	
* Write access to the Keybus requires an NPN transistor and a resistor.  Most small signal NPN transistors should be suitable.
* When powering the ESP from an external power supply such as USB (recommended) ensure you still connect the ground to the DSC panel if you are not using the isolated circuit version.
	
## OTA updates
In order to make OTA updates with an ESP8266, the connection switch in frontend must be switched to OFF since the DSC library is using interrupts. This is not normally necessary with an ESP32.

## References
* ESPHOME yaml and component structure based on code from https://github.com/Margriko/Paradox-ESPHome
* Wiring and DSC library from https://github.com/taligentx/dscKeybusInterface
* ESPHOME diy examples from https://esphome.io/guides/diy.html


