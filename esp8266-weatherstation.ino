#include <ESP8266WiFi.h>
#include "myconfig.h"
#include "Output_relay.h"
#include "1-wire_temperature.h"
#include "OTAupdate.h"
// #include <ESP8266HTTPClient.h>
// #include <ESP8266httpUpdate.h>
#include <PubSubClient.h>

// please install the library PubSubClient, TaskScheduler
#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#include <TaskScheduler.h>

unsigned long  next_timestamp = 0;
volatile unsigned long i = 0;
float wind = 0;
float last_wind = 0;
int count = 0;
volatile unsigned long last_micros;
long debouncing_time = 5; //in millis
//int input_pin_wind = 13;
char charBuffer[32];

WiFiClient espClient;
PubSubClient client(espClient);

void ICACHE_RAM_ATTR Interrupt()
{
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    i++;
    last_micros = micros();
  }
}

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
    Serial.print("task Wind: ");
    Serial.println(millis());
    
  detachInterrupt(input_pin_wind);
    count++; 
    if(i == 0)
      wind = 0.0;
    else
      wind = (i * 0.8) + 3; // PCE Instruments PCE-WS P 
    if(last_wind - wind > 0.8 || last_wind - wind < -0.8 || count >= 10){
      if(debugOutput){
        Serial.print("Wind: ");
        Serial.print(wind);
        Serial.println(" km/h");
      }
      String strBuffer;
      strBuffer =  String(wind);
      strBuffer.toCharArray(charBuffer,10);
      if (!client.publish(mqtt_topic_prefix, charBuffer, false))
      {
        if(debugOutput) Serial.print("Can not publish to MQTT Broker");
      }
      
      count = 0;
    }
    i = 0;
    last_wind = wind;

    attachInterrupt(input_pin_wind,Interrupt,RISING);
}

void tMQTTCallback() {
    Serial.print("task MQTT: ");
    Serial.println(millis());
    // do_update(); OTA update disabled
    client.setServer(mqtt_host, mqtt_port);

    if (!client.connected()) {
      reconnect();
    }
      client.loop();
}

void tRelayOutCallback(){
   Serial.print("task Relay output: ");
   //Serial.println(millis());
   wind_max(wind);
}
  
void t_1_wire_measureCallback(){
  if (debugOutput){
    Serial.print("task 1-Wire temprature: "); 
    Serial.println(millis());
  }
  one_wire_measure(); 
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

  // set priority, higher is better
  tMQTT.setId(100);
  t_1_wire_measure.setId(150);
  tWind.setId(600);
  tRelayOut.setId(601);
  
  delay(500);

  runner.setHighPriorityScheduler(&runnerHPR); 
  runner.enableAll(true); // this will recursively enable the higher priority tasks as well
  
  client.setServer(mqtt_host, mqtt_port);
//  do_update();

  Init_temprature();
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
      client.subscribe("inTopic");
    } else {
      if(debugOutput){
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}
