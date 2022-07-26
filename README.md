# Description
This project measures the wind in km/h and reports the value via MQTT.
I used that for retrieving my blind if the wind is 8 BFT (70 km/h) or higher.

# Hardware
The following hardware is used for building this project:
* ESP8266 (Wemos D1)
* PCE Instruments PCE-WS P Anemometer https://www.conrad.de/de/p/pce-instruments-pce-ws-p-anemometer-2452812.html

# Wiring
I connected the anemometer with the D7 and GND Pin of my Wemos D1. But it can be all other suitable Pins used.

# Software
For using this software you have to copy the myconfig_samples.h to myconfig.h and change the values for the MQTT server and the WLAN settings.
