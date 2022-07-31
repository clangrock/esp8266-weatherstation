/* one-wire temperature (DS18B20)
 *  
 * Author:  Christian Langrock
 * Date:    2022-July-31
 */


#include <OneWire.h>            // OneWire-Bibliothek einbinden
#include <DallasTemperature.h>  // DS18B20-Bibliothek einbinden
#include "datatypes.h"


DeviceAddress Thermometer[64];

// Assign address manually. The addresses below will need to be changed
// to valid device addresses on your bus. Device address can be retrieved
// by using either oneWire.search(deviceAddress) or individually via
// sensors.getAddress(deviceAddress, index)
// DeviceAddress insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
// DeviceAddress outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

OneWire oneWire(DS18B20_PIN);          // OneWire Referenz setzen
DallasTemperature sensors(&oneWire);   // DS18B20 initialisieren

char* strMQTTTopic_Temp;
char* chrSubTopic_Temp {"/temp_"};


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
float read_Temperature(int deviceNumber){

  if (deviceNumber <= numberOfDevices){
      float  tempC = sensors.getTempC(Thermometer[deviceNumber]);
      if (tempC == DEVICE_DISCONNECTED_C)
        {
          if (debugOutput) Serial.println("Error: Could not read temperature data");
          return 0.0;
        }
      else{
       if (debugOutput){
          Serial.print("Temp C: ");
          Serial.print(tempC);
          Serial.print(" ");  // Hier müssen wir ein wenig tricksen
          Serial.write(176);  // um das °-Zeichen korrekt darzustellen
          Serial.println("C");
          }
        return tempC;
      }
    }
  return 0.0;
}

int Init_temprature() {  
  sensors.begin();  // DS18B20 starten
  strMQTTTopic_Temp = mqtt_topic_prefix;
  strcpy(chrSubTopic_Temp, "/Temp_");
  strcpy(strMQTTTopic_Temp, mqtt_topic_prefix);
  strcat(strMQTTTopic_Temp, chrSubTopic_Temp);
    if(debugOutput){
    Serial.print("Prefix MQTT temperature: ");
    Serial.println(chrSubTopic_Temp);
    Serial.println(mqtt_topic_prefix);
    Serial.println(strMQTTTopic_Temp);
  }
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("1- wire Sensoren: "); 
  Serial.println(numberOfDevices, DEC);

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()){ 
    Serial.println("ON");
  }
  else Serial.println("OFF");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(Thermometer[i], i))
    {
       if(debugOutput){
        Serial.print("Found device ");
        Serial.print(i, DEC);
        Serial.print(" with address: ");
        printAddress(Thermometer[i]);
        Serial.println();
        Serial.print("Setting resolution to ");
        Serial.println(TEMPERATURE_PRECISION, DEC);
        }
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(Thermometer[i], TEMPERATURE_PRECISION);
      if(debugOutput){
          Serial.print("Resolution actually set to: ");
          Serial.print(sensors.getResolution(Thermometer[i]), DEC);
          Serial.println();
      }
    } else {
      if(debugOutput){
        Serial.print("Found ghost device at ");
        Serial.print(i, DEC);
        Serial.print(" but could not detect address. Check power and cabling");
        }
    }
  }

  return numberOfDevices;
}


// as you read the device addresses with one of the many examples call this procdure
// to store the Device Address in the DS18B20_DeviceByteAddresses array.

struct DeviceAddress_array Temperature_storeDeviceAddress(void) {
  // function to convert a device address in a printable string
  struct DeviceAddress_array outAddress;
  for (uint8_t j = 0; j < numberOfDevices; j++){
      for (uint8_t i = 0; i < 8; i++) {
        outAddress.Address[j][i] = Thermometer[j][i];
      }
  }
  return outAddress;
}
