# DSCKeyBus ESPHOME custom component to interface to a DSC POWERSERIES alarm system.

If you like this project and wish to supplement my coffee intake, please click the button below to donate! Thank you!

[!["Buy Me A Coffee"](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://www.buymeacoffee.com/Dilbert66)

This is an implementation of an ESPHOME custom component to interface directly to a DSC POWERSERIES (Classic series not supported) alarm system using the keybus interface and  very inexpensive ESP8266/ESP32 modules . NOTE: I strongly suggest using an ESP32 for it's increased capabilities. 

Using a heavily modified DSC keybus interface library from https://github.com/taligentx/dscKeybusInterface, it provides full read and write control of the alarm system. The idea and yaml code is based on the DIY example at: https://github.com/Margriko/Paradox-ESPHome.

This library is designed around the "external components" feature of ESPHome (https://esphome.io/components/external_components.html).  The previous version of this library was using the older "custom components" feature which is being deprecated.  Using external components is a much better alternative and allows for much easier install and customization.

To install, first copy the DscAlarm.yaml file from here to your esphome directory. You can then edit it either directly or using the ESPHome dashboard to match your alarm panel setup.  The yaml is self explanatory.  Secondly, for the main library support code installation you have two choices:

 1: You can leave the yaml external_components source line uncommented and it will automatically fetch the required code from my esphome-components repository during the compile.  
 
 or alternatively:
 
 2: If you prefer a more manual approach, you can comment out the github source line and uncomment the source "my_components" line.  You will then need to create a matching my_components directory in your esphome home path.  Name it anything you like as long as it matches the source line in the yaml.  You then copy all content from the "components" directory located in repository https://github.com/Dilbert66/esphome-components/tree/main/ to this new directory. Please note that for any future updates, you will again need to do the copy process.

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
A new service is also made available in home assistant called  esphome.\<yourdscsystemname\>_set_zone_fault with attributes zone and fault: 
zone is 2 digit zone number and fault is 0 or 1 where 1 sets a zone as open and 0 closes it. By calling this service from a home assistant function, you can enable any event, sensor, etc to trigger any one of your emulated zones in your alarm system.

Relay module support is also added.   This will give your panel the ability to trigger an event on any PGM channel output.  See the yaml file for an example configuration.  An example output for pgm 1 will show in home assistant as <systemname> PGM 1. You can add more fields in the yaml config file.

This version now also adds the ability to display the low battery warning status of any wireless zones.  These will be showng in the "zone status" field of the yaml.   Also in that field, you will see the alarm status, open and bypass statuses of any zones
The display format for a low battery is BL:zz and alarm status will be shown as AL:zz  where zz is the zone number.  Bypass is BY:xx and open is OP:xx.

I've also added a custom alarm panel card with new  functions from the library that provides similar functions as available from an LCD keypad.  This includes the display menus and the ability to fully program your system using full text display of all fields.
	
## Example in Home Assistant
![Image of HASS example](https://github.com/Dilbert66/esphome-dsckeybus/blob/master/dsckeybusinterface.png)

The returned statuses for Home Assistant are: armed_away, armed_home, armed_night, pending, disarmed,triggered and unavailable.  

	
Sample Home Assistant Template Alarm Control Panel configuration with simple services (defaults to partition 1).

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

Alternative Home assistant configuration using specified partition with set_alarm_state service.

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
I've added a sample lovelace alarm-panel card copied from the repository at https://github.com/GalaxyGateway/HA-Cards. I've customized it to work with this ESP library's services.   I've also added two new text fields that will be used by the card to display the panel prompts the same way a real keypad does. To configure the card, just place the `alarm-keypad-card.js` and `*.mp3` files from the ha_keypad_card folder into the `/config/www` directory of your homeassistant installation and add a new resource in your lovelace configuration pointing to `/local/alarm-keypad-card.js`. <br>
Add a reference to alarm-keypad-card.js in Lovelace. There’s two ways to do that:<br>
1. Using UI:  go to settings (cog icon bottom left in sidebar menu), click on 'dashboards', click on top right 3 dots option then on Resources,
  bottom right 'add resource' option, in window, paste the url as '/local/alarm-keypad-card.js?id=1', select javascript module.

2. Using YAML: Add following code to lovelace section

```yaml
resources:
- url: /local/alarm-keypad-card.js?id=1
  type: module
```
*** NOTE *** :  Every time you update this file, you will need to increase the id=xx number in the url by 1 in order to force the browser to reload it.

### Panel Card example
	
![image](https://github.com/user-attachments/assets/db4bdf2c-ed17-4ee0-b33e-f0bd9359d61a)



### Panel card example lovelace configuration
```
type: custom:alarm-keypad-card
title: DSC Keypad
disp_line1: sensor.dscalarm_ln1_1
disp_line2: sensor.dscalarm_ln2_1
service_type: esphome
service: dscalarm_alarm_keypress_partition
status_A: READY
status_B: ARMED
status_C: TROUBLE
status_D: AC
sensor_A: binary_sensor.dscalarm_rdy_1
sensor_B: binary_sensor.dscalarm_arm_1
sensor_C: binary_sensor.dscalarm_tr
sensor_D: binary_sensor.dscalarm_ac
status_A_off_icon: mdi:circle-off-outline
status_A_on_icon: mdi:check-circle
status_A_color: green
status_C_on_icon: mdi:alert
status_C_off_icon: mdi:triangle-outline
status_C_color: orange
status_B_on_icon: mdi:shield-lock
status_B_off_icon: mdi:shield-off-outline
status_B_color: red
status_D_on_icon: mdi:power-plug
status_D_off_icon: mdi:power-plug-off-outline
status_D_color: green
#optional styling and colors
style:
  "--lcdbgcolordark": var(--input-fill-color)
  "--lcdtextcolordark": var(--primary-text-color)
  "--buttonbgcolordark": var(--input-fill-color)
  "--buttontextcolordark": var(--primary-color)
  "--buttonhovercolordark": var(--outline-hover-color)
  "--buttonactivecolordark": var(--secondary-text-color)
  "--sensorlabelcolordark": var(--accent-color));
  "--buttonfocuscolordark": var(--outline-color)
  "--sensoroffcolordark": var(--disabled-text-color)
  "--bordercolordark": var(--disabled-color)

  "--sensoroffcolorlight": var(--disabled-text-color)
  "--lcdbgcolorlight": var(--input-fill-color)
  "--lcdtextcolorlight": var(--primary-text-color)
  "--sensorlabelcolorlight": var(--accent-color));
  "--buttonbgcolorlight": var(--input-fill-color)
  "--buttontextcolorlight": var(--primary-color)
  "--buttonhovercolorlight": var(--outline-hover-color)
  "--buttonactivecolorlight": var(--secondary-text-color)
  "--buttonfocuscolorlight": var(--outline-color)
  "--bordercolorlight": var(--disabled-color)
button_A: STAY
button_B: AWAY
button_C: NIGHT
button_D: CHIME
button_E: ""
button_F: <
button_G: ">"
button_H: ""
cmd_A:
  keys: S
  partition: 1
cmd_B:
  keys: W
  partition: 1
cmd_C:
  keys: "N"
  partition: 1
cmd_D:
  keys: C
  partition: 1
cmd_F:
  keys: <
  partition: 1
cmd_G:
  keys: ">"
  partition: 1
key_0:
  keys: "0"
  partition: 1
key_1:
  keys: "1"
  partition: 1
key_2:
  keys: "2"
  partition: 1
key_3:
  keys: "3"
  partition: 1
key_4:
  keys: "4"
  partition: 1
key_5:
  keys: "5"
  partition: 1
key_6:
  keys: "6"
  partition: 1
key_7:
  keys: "7"
  partition: 1
key_8:
  keys: "8"
  partition: 1
key_9:
  keys: "9"
  partition: 1
key_star:
  keys: "*"
  partition: 1
key_pound:
  keys: "#"
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
text_0: ""
beep: binary_sensor.dscalarm_bp_1
button_disabled_A: false
button_disabled_B: false
button_disabled_C: false
button_disabled_D: false
button_disabled_E: true
button_disabled_F: false
button_disabled_G: false
button_disabled_H: true
scale: 1
left_buttons: false
view_bottom: true

	
```	

![dscinterface](https://user-images.githubusercontent.com/7193213/135171111-a1f723cf-43fc-4d4a-bdb9-61645630c7be.png)

## MQTT Support
If you would like to use MQTT communications with Homeassistant or alternatively use ESPHOME with other platforms that can support MQTT, you can modify the configuration to use the MQTT protocol instead of the native HA API integration.  This simply involves the addtion of an mqtt: configuration section in the yaml and to remove the api: section.   Please see the ESPHOME MQTT documentation more details: https://esphome.io/components/mqtt.html .  For an example on how to configure the alarm-panel-card to use MQTT services, please see the lovelace.yaml file.  

Command topic is "&lt;yoursystemnameinesphome&gt;/alarm/set"
	
Command payload for sending key commands: {"keys":"&lt;sequenceofkeys&gt;","partition":&lt;partition#&gt;}
	
Command payload to set an expander fault status: {"zone":&lt;zone#&gt;,"fault":&lt;on:off or 0:1&gt;}
	
Command payload to set an alarm state: {"state":"&lt;alarmstate&gt;","partition":&lt;partition#&gt;,"code":"&lt;accesscode&gt;"} - see set_alarm_state api command above for more details

Sensor data will follow the HA MQTT discovery format. See here for details: https://www.home-assistant.io/docs/mqtt/discovery/

If you prefer to use Arduino instead of ESPHome, I've also provided an Arduino example sketch in the mqtt_example folder.  Follow the instructions at the top of the file.
	
## Wiring
### Isolated version
![image](https://user-images.githubusercontent.com/7193213/193299136-eb453cc3-6b2b-4224-a018-fefc0edb01b1.png)
### Non-isolated simple version	
![Image of Wiring schematic](https://github.com/Dilbert66/esphome-dsckeybus/blob/master/dscinterface.png)

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


