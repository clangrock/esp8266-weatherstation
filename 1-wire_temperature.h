/* one-wire temperature (DS18B20)

   Author:  Christian Langrock
   Date:    2022-August-06
*/

#include <OneWire.h>            // OneWire-Bibliothek einbinden
#include <DallasTemperature.h>  // DS18B20-Bibliothek einbinden

DeviceAddress Thermometer[64];

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

int numberOfDevices; // Number of temperature devices found

// arrays to hold device addresses
struct DeviceAddress_array Thermometers;

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

OneWire oneWire(DS18B20_PIN);          // OneWire Referenz setzen
DallasTemperature sensors(&oneWire);   // DS18B20 initialisieren

char* strMQTTTopicTemp {mqtt_topic_temperature};

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// function to print the temperature for a device
float read_Temperature(int deviceNumber) {

  if (deviceNumber <= numberOfDevices) {
    float  tempC = sensors.getTempC(Thermometer[deviceNumber]);
    if (tempC == DEVICE_DISCONNECTED_C)
    {
      if (debugOutput || debugError) Serial.println("Error: Could not read temperature data");
      return 0.0;
    }
    else {
      if (debugMeasure) {
        Serial.print("Temp: ");
        Serial.print(tempC);
        Serial.println("°C");
      }
      return tempC;
    }
  }
  return 0.0;
}

// as you read the device addresses with one of the many examples call this procdure
// to store the Device Address in the DS18B20_DeviceByteAddresses array.

struct DeviceAddress_array Temperature_storeDeviceAddress(void) {
  // function to convert a device address in a printable string
  struct DeviceAddress_array outAddress;
  for (uint8_t j = 0; j < numberOfDevices; j++) {
    for (uint8_t i = 0; i < 8; i++) {
      outAddress.Address[j][i] = Thermometer[j][i];
    }
  }
  return outAddress;
}

int Init_temperature() {
  sensors.begin();  // DS18B20 starten

  // build MQTT topic
  if (debugOutput) {
    Serial.print("MQTT topic prefix for temperature: ");
    Serial.println(strMQTTTopicTemp);
  }
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  if (debugOutput) {
    Serial.print("1- wire Sensoren: ");
    Serial.println(numberOfDevices, DEC);

    // report parasite power requirements
    Serial.print("Parasite power is: ");
    if (sensors.isParasitePowerMode()) {
      Serial.println("ON");
    }
    else Serial.println("OFF");
  }
  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(Thermometer[i], i)) {
      if (debugOutput) {
        Serial.print("Found device ");
        Serial.print(i, DEC);
        Serial.print(" with address: ");
        printAddress(Thermometer[i]);
        Serial.println();
        Serial.print("Setting resolution to ");
        Serial.println(TEMPERATURE_PRECISION, DEC);
      }
      // copy one-wire addresses to a byte array
      Thermometers = Temperature_storeDeviceAddress();
      if (debugOutput) {
        for (uint8_t i = 0; i < numberOfDevices; i++ ) {
          Serial.print("Sensor Adress ");
          Serial.print(i);
          Serial.print(": ");
          for (uint8_t y = 0; y < 8; y++) {
            Serial.print(Thermometers.Address[i][y], HEX);
          }
          Serial.println(" ");
        }
      }

      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(Thermometer[i], TEMPERATURE_PRECISION);
      if (debugOutput) {
        Serial.print("Resolution actually set to: ");
        Serial.print(sensors.getResolution(Thermometer[i]), DEC);
        Serial.println();
      }
    } else {
      if (debugOutput || debugError) {
        Serial.print("Found ghost device at ");
        Serial.print(i, DEC);
        Serial.print(" but could not detect address. Check power and cabling");
      }
    }
  }

  return numberOfDevices;
}
