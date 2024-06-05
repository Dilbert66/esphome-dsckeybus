# Crack Installer Code

There are now 2 versions of this code. One that works using MQTT and the other using the built ESPHOME web_server component on the ESP.

MQTT version: 

This Python script does a brute force attack to try and find the installer code. 
The ESP32 needs to be set up to use MQTT, as this script uses MQTT to send and receive messages via the ESP32.

This script can run on any computer with access to Home Assistant's MQTT broker.
This script is best run in a virtual environment, or open this folder in PyCharm.
It also runs fine in Windows's python environment.
After cloning or downloading this repo, switch to the `Crack_Installer_Code` directory and run `python3 -m venv .venv`, 
then activate the environment with `source .venv/bin/activate`
The only dependency of this script is `paho-mqtt`. You can do a `pip -i requirements.txt` to install it.

At the top of `crack_dsc_mqtt.py` change the MQTT credentials to use you Home Assistant MQTT instance. 
Change the `device_name` to the ESPHome device's name.
You can configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

On my alarm I was able to test 10 codes every 120 seconds. That means going through the whole 0000 to 9999 range will take quite a few hours.
The reason for this is the keypad lockout that happens when too many invalid codes are entered. 
After waiting for the remainder of the 2 minutes, the lockout is cleared an one can continue the attack.

Web API version:

The ESP32 needs to have the custom web_server component installed from the repository github://Dilbert66/esphome-components@dev, as this script uses the REST API of this component.

Use these entries in your yaml config file to set it up:

external_components:
  - source: github://Dilbert66/esphome-components@dev 
    components: [dsc_alarm_panel,template_alarm,web_server,mg_lib] 
    refresh: 10s

mg_lib:

web_server:
  port: 80  
  partitions: 1
  log: false
  local: true
  config_url: https://dilbert66.github.io/config_files/config_dsc.yml
  js_url: https://dilbert66.github.io/js_files/www.js

Also make sure that you have this entry in your yaml text_sensor: section:

# partition message id code = (msg_ + partition number) 
  - platform: template_alarm
    id: msg_1
    name: "Partition 1 Msg (msg_1)"

This script is best run in a virtual environment, or open this folder in PyCharm.
It also runs fine in Windows's python environment.
Simply run the crack_dsc_web.py from the commmand line.
The only dependency of this script is `aiosseclient`. You can install it with `pip3 install aiosseclient`

Change the `device_name` to the ESPHome device's name.

You can configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

