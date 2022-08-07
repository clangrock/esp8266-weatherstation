# Description
This project measures the wind in km/h and reports the value via MQTT.
I used that for retrieving my blind if the wind is 8 BFT (70 km/h) or higher.

# Hardware
The following hardware is used for building this project:
* ESP8266 (Wemos D1)
* PCE Instruments PCE-WS P Anemometer https://www.conrad.de/de/p/pce-instruments-pce-ws-p-anemometer-2452812.html
* Power supply 230V AC to 5V DC: https://www.az-delivery.de/products/copy-of-220v-zu-5v-mini-netzteil
* Rain sensor: https://www.az-delivery.de/products/regen-sensor-modul
* Relays module https://www.az-delivery.de/products/2-relais-modul
* Temperature sensors DS18B20: https://www.az-delivery.de/products/2xds18b20wasserdicht 

# Wiring
see the wireing diagramm: wire_diagramm.pdf

# Software
For using this software you have to copy the myconfig_samples.h to myconfig.h and change the values for the MQTT server and the WLAN settings.
