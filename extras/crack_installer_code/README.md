# Crack Installer Code

There are 3 variations of this script. One that works using the ESPHOME API, one using ESPHOME MQTT and the other using the Esphome custom web_keypad component.

ESPHOME API version:

If using the esphome API, you can use version which uses the python aioesphomeapito library module to talk to your ESPHOME device. It provides full standalone access to your ESP without the need for any external systems.

Just pick your favorite Python 3.x instance in Linux or Windows and run it. It runs fine in Windows's python environment from here: https://www.python.org/downloads/windows/

The is only one dependency for this script which is  `aioesphomeapito` .
You can install it with `pip3 install aioesphomeapito`.

Change the `device_address` at the top of the crack_dsc_api.py script to the ESPHome device's name.
Change the `passkey` to your secret ESPHOME encryption key
Change the `sensor_id` to the name of the sensor in your yaml used for partition messages (id: msg_1) (if you have changed it , otherwise leave as is)

You can configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

The system should be able to complete the full sequence of 10000 numbers in a few hours. 

Simply run the script from the commmand line using "python crack_dsc_api.py"


MQTT version: 

This Python script does a brute force attack to try and find the installer code. 
The ESP32 needs to be set up to use MQTT, as this script uses MQTT to send and receive messages via the ESP32.

Just pick your favorite Python 3.x instance in Linux or Windows and run it. It runs fine in Windows's python environment from here: https://www.python.org/downloads/windows/

The is one dependency for this script which is the `paho-mqtt` module.
You can install it with the pip command `pip3 install paho-mqtt`.

At the top of `crack_dsc_mqtt.py` change the MQTT credentials and address to use for your Home Assistant MQTT instance.  Also change the `device_name` to the ESPHome device's name.

You can also configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

The system should be able to complete the full sequence of 10000 numbers in a few hours. 

Simply run the script from the commmand line using "python crack_dsc_mqtt.py"



Web API version (ESP32 only):
If you don't use mqtt, I offer this version which uses the web_keypad module's REST api to talk to your DSC panel. It provides full standalone access to your ESP without the need for any external systems.

You will first need to be using an ESP32 and be using the custom web_server component installed from the repository github://Dilbert66/esphome-components@dev. Have a look at the example DSCAlarm.yaml file from that repository for more info.  The main changes needed are as noted below.

Use these additional entries in your DSC yaml config file to set it up. Note the addition of "web_keypad and mg_lib components to the list:

external_components:
  - source: github://Dilbert66/esphome-components@dev 
    components: [dsc_alarm_panel,template_alarm,web_keypad,mg_lib] 
    refresh: 10s

mg_lib:

#default web server setup.
web_server:
  port: 80  
  partitions: 1
  log: false
  config_url: https://dilbert66.github.io/config_files/config_dsc.yml
  js_url: https://dilbert66.github.io/js_files/www.js

Also make sure that you also have this entry  (id: msg_1) in your yaml text_sensor: section:

# partition message id code = (msg_ + partition number) 
  - platform: template_alarm
    id: msg_1
    name: "Partition 1 Message"

Just pick your favorite Python 3.x instance in Linux or Windows and run it. It runs fine in Windows's python environment from here: https://www.python.org/downloads/windows/

The are 2 dependencies for this script. First is `aiosseclient` and the other is `aiohttp`.
You can install them with `pip3 install aiosseclient` and `pip3 install aiohttp`

Change the `device_name` at the top of the crack_dsc_web.py script to the ESPHome device's name.

You can configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

The system should be able to complete the full sequence of 10000 numbers in a few hours. 

Simply run the script from the commmand line using "python crack_dsc_web.py"

