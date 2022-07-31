/* Weatherstation
 * PCE Instruments PCE-WS P 
 * Author:  Christian Langrock
 * Date:    2022-July-31
 */

#include <ESP8266WiFi.h>
#include "myconfig.h"
#include "Output_relay.h"
#include "1-wire_temperature.h"
#include "OTAupdate.h"
#include "windsensor.h"
//#include "datatypes.h"
#include <PubSubClient.h>

// please install the library PubSubClient, TaskScheduler
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#include <TaskScheduler.h>

float windspeed  {0.0};
float last_wind_MQTT  {0.0};
int count_MQTT_Wind {0};
float returnTemperature {0.0};
int numberTemperatureSensor {0};


// arrays to hold device addresses
struct DeviceAddress_array Thermometers;

WiFiClient espClient;
PubSubClient client(espClient);


// ************************* Tasks ******************************

Scheduler runner;
Scheduler runnerHPR; // high priority tasks


// Callback methods prototypes
void tWindCallback();
void tMQTTCallback();
void tRelayOutCallback();
void t_1_wire_measureCallback();

Task tWind(1000, TASK_FOREVER, &tWindCallback, &runnerHPR);
Task tMQTT(5000, TASK_FOREVER, &tMQTTCallback, &runner);
Task tRelayOut (2000, TASK_FOREVER, tRelayOutCallback, &runnerHPR);
Task t_1_wire_measure (5000, TASK_FOREVER, t_1_wire_measureCallback, &runner);

void tWindCallback() {
  if (debugOutput){
    Serial.print("task Wind: ");
    Serial.println(millis());
  }
    // read windsensor
    windspeed = windsensor();
 
}

// MQTT communication
void tMQTTCallback() {
  if (debugOutput){
    Serial.print("task MQTT: ");
    Serial.println(millis());
  }
    // do_update(); OTA update disabled
    client.setServer(mqtt_host, mqtt_port);

    if (!client.connected()) {
      reconnect();
    }
       // publish wind speed
       count_MQTT_Wind++;
       if (last_wind_MQTT - windspeed > 0.8 || last_wind_MQTT - windspeed < -0.8 || count_MQTT_Wind >= 10){
             MQTT_puplish_float (strMQTTTopic_wind, windspeed);
             count_MQTT_Wind = 0;
       }

      client.loop();
     
}

// relay output
void tRelayOutCallback(){
  if (debugOutput){
   Serial.print("task Relay output: ");
   Serial.println(millis());
  }
   wind_max(windspeed);
}
  
void t_1_wire_measureCallback(){

  char* chrBuffer_Temp {""};
  char chrNumber[2];

  if (debugOutput){
    Serial.print("task 1-Wire temprature: "); 
    Serial.println(millis());
      Serial.print("Prefix: ");
  Serial.println(strMQTTTopic_Temp);
  }
  int deviceNumber {0};
  
  for (uint8_t i = 0; i < numberTemperatureSensor; i++){
    returnTemperature = 0.0;
    returnTemperature = read_Temperature(i); 
    chrBuffer_Temp = strMQTTTopic_Temp;
    //chrNumber[1] = "";
    String strBufferT = String(i);
    strBufferT.toCharArray(chrNumber,2);
    strcat(chrBuffer_Temp, chrNumber);
    Serial.println(chrBuffer_Temp);
    if (debugOutput){
      Serial.print("Temp_");
      Serial.print(i, DEC);
      Serial.print(": ");
      Serial.println(returnTemperature, DEC);
    }
    MQTT_puplish_float (chrBuffer_Temp, returnTemperature);
  }
  /*
      char* strMQTTTopic_temp {mqtt_topic_prefix};
      char* chrSubTopic {"/temp"};
      char chrNumber[6];
      String strBuffer;
      strBuffer = String(number);
      strBuffer.toCharArray(chrNumber,6);

      strcat(strMQTTTopic_temp, chrSubTopic);
      strcat(strMQTTTopic_temp,"_");
      strcat(strMQTTTopic_temp, chrNumber);
  */
}


// ********************  WIFI  ********************

// More events: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiGeneric.h

void onSTAConnected(WiFiEventStationModeConnected e /*String ssid, uint8 bssid[6], uint8 channel*/) {
  Serial.printf("WiFi Connected: SSID %s @ BSSID %.2X:%.2X:%.2X:%.2X:%.2X:%.2X Channel %d\n",
    e.ssid.c_str(), e.bssid[0], e.bssid[1], e.bssid[2], e.bssid[3], e.bssid[4], e.bssid[5], e.channel);
 }

void onSTADisconnected(WiFiEventStationModeDisconnected e /*String ssid, uint8 bssid[6], WiFiDisconnectReason reason*/) {
  Serial.printf("WiFi Disconnected: SSID %s BSSID %.2X:%.2X:%.2X:%.2X:%.2X:%.2X Reason %d\n",
    e.ssid.c_str(), e.bssid[0], e.bssid[1], e.bssid[2], e.bssid[3], e.bssid[4], e.bssid[5], e.reason);
  // Reason: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFiType.h
}

void onSTAGotIP(WiFiEventStationModeGotIP e /*IPAddress ip, IPAddress mask, IPAddress gw*/) {
  Serial.printf("WiFi GotIP: localIP %s SubnetMask %s GatewayIP %s\n",
    e.ip.toString().c_str(), e.mask.toString().c_str(), e.gw.toString().c_str());
}


void initWiFi(){
  static WiFiEventHandler e1, e2, e4;
  WiFi.disconnect(true);
  delay(1000);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
    // WiFi events
  e1 = WiFi.onStationModeConnected(onSTAConnected);
  e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
  e4 = WiFi.onStationModeGotIP(onSTAGotIP);
  WiFi.begin(ssid, password);   // WiFi connect
  Serial.println("Waiting for WIFI network...");
}


void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(input_pin_wind, INPUT_PULLUP);//D4
  pinMode(output_pin_Relay_1, OUTPUT); // D5 
  pinMode(output_pin_Relay_2, OUTPUT); // D6 
  // We start by connecting to a WiFi network
  initWiFi();

  initWindsensor();

  // set priority, higher is better
  tMQTT.setId(50);
  t_1_wire_measure.setId(150);
  tWind.setId(600);
  tRelayOut.setId(500);
  
  delay(5000);

  runner.setHighPriorityScheduler(&runnerHPR); 
  runner.enableAll(true); // this will recursively enable the higher priority tasks as well
  
  client.setServer(mqtt_host, mqtt_port);
//  do_update();

  numberTemperatureSensor = Init_temprature();
 // copy on-wire addresses to a byte array
 Thermometers = Temperature_storeDeviceAddress();
 if (debugOutput){
    for (uint8_t i = 0; i < numberTemperatureSensor; i++ ){
      Serial.print("Sensor Adress ");
      Serial.print(i);
      Serial.print(": ");
      for (uint8_t y = 0; y < 8; y++){
        Serial.print(Thermometers.Address[i][y], HEX);
      }
      Serial.println(" ");
    }
 }
  reconnect();
  
  attachInterrupt(input_pin_wind,Interrupt,RISING);
}


void loop() 
{
  // Task execute
  runner.execute();

  yield();
}


void reconnect() {
  // Loop until we're reconnected
  if (!client.connected()) {
    if(debugOutput) Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mqtt_id + String("-");
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      if(debugOutput) Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "hello world");
      // ... and resubscribe
      //client.subscribe("inTopic");
    } else {
      if(debugOutput){
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}

bool MQTT_puplish_float (char* strMQTTTopic, float fMQTTValue) {
      bool Com_Ok  {false};
      char charBuffer[32];
      String strBuffer;
      strBuffer =  String(fMQTTValue);
      strBuffer.toCharArray(charBuffer,10);
      
      if (!client.publish(strMQTTTopic, charBuffer, false))
      {
        if(debugOutput) {
          Serial.print("Can not publish to MQTT Broker: ");
          Serial.print(strMQTTTopic);
          Serial.print(" ");
        }
      }
      else{
        Com_Ok = true;
      }
      return Com_Ok;
}
