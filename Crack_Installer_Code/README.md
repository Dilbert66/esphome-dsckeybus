# Crack Installer Code

This Python script does a brute force attack to try and find the installer code. 
The ESP32 needs to be set up to use MQTT, as this script uses MQTT to send and receive messages via the ESP32.

This script can run on any computer with access to Home Assistant's MQTT broker.
This script is best run in a virtual environment, or open this folder in PyCharm.
After cloning or downloading this repo, switch to the `Crack_Installer_Code` directory and run `python3 -m venv .venv`, 
then activate the environment with `source .venv/bin/activate`
The only dependency of this script is `paho-mqtt`. You can do a `pip -i requirements.txt` to install it.

At the top of `main.py` change the MQTT credentials to use you Home Assistant MQTT instance. 
Change the `device_name` to the ESPHome device's name.
You can configure a start and end code for the range of codes to test. This is useful if you want to stop and restart later.

On my alarm I was able to test 10 codes every 120 seconds. That means going through the whole 0000 to 9999 range will take 33.33 hours.
The reason for this is the keypad lockout that happens when too many invalid codes are entered. 
After waiting for the remainder of the 2 minutes, the lockout is cleared an one can continue the attack.