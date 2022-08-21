/* Weatherstation
   PCE Instruments PCE-WS P
   Author:  Christian Langrock
   Date:    2022-August-21
   Version: V0.8.2
*/

int ESP8266status {0}; // 0 = startup; 1 = normal run; 2 = WIFI error

// please install the library PubSubClient, TaskScheduler
#include <Arduino.h>
#include "datatypes.h"
#include "myconfig.h"
#include "statusLed.h"
#include "wifi.h"
// functions for MQTT Client
#include "MQTT.h"

// I/O-functions
#include "Output_relay.h"
#include "1-wire_temperature.h"
#include "OTAupdate.h"
#include "windsensor.h"
#include "rain_sensor.h"

// Task scheduler
#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_PRIORITY
#define _TASK_WDT_IDS
#define _TASK_TIMECRITICAL
#include <TaskScheduler.h>

float windspeed  {0.0};
float last_windspeed  {0.0};
float last_wind_MQTT  {0.0};
int count_MQTT_Wind {0};
int count_MQTT_reconnect {0};


int numberTemperatureSensor {0};
float temperatures_old[64] {0.0};
struct Rain_array RainResult_old;


// ************************* Tasks ******************************

Scheduler runner;
Scheduler runnerHPR; // high priority tasks
Scheduler runnerCPR; // critical priority tasks


// Callback methods prototypes
void tWindCallback();
void tMQTTCallback();
void tRelayOutCallback();
void t_1_wire_measureCallback();
void tRainSensorCallback();
void tLEDCallback();

Task tWind            (1000, TASK_FOREVER, &tWindCallback,            &runnerCPR);
Task tMQTT            (5000, TASK_FOREVER, &tMQTTCallback,            &runner);
Task tRelayOut        (2000, TASK_FOREVER, tRelayOutCallback,         &runnerCPR);
Task t_1_wire_measure (3560, TASK_FOREVER, t_1_wire_measureCallback,  &runnerHPR);
Task tRainSensor      (5010, TASK_FOREVER, tRainSensorCallback,       &runnerHPR);
Task tLED             (250,  TASK_FOREVER, tLEDCallback,              &runner);

// task for wind sensor
void tWindCallback() {
  // read windsensor
  windspeed = windsensor();
  
// if the windspeed increases very fast, trigger the relay task
  if (((windspeed + 30.0) > last_windspeed) && (windspeed > Config_max_max_wind_speed ) ) {
    wind_max_max();
  }
 last_windspeed = windspeed;
}

// task for MQTT communication
void tMQTTCallback() {
  // reconnect only after 10 runs
  if (count_MQTT_reconnect >= 10) {
    if (!client.connected()) {
      reconnect();
    }
    count_MQTT_reconnect = 0;
  }
  
  // publish wind speed
  count_MQTT_Wind++;
  if (last_wind_MQTT - windspeed > 0.8 || last_wind_MQTT - windspeed < -0.8 || count_MQTT_Wind >= 6) {
    MQTT_puplish_float (strMQTTTopic_wind, windspeed);
    count_MQTT_Wind = 0;
  }
  count_MQTT_reconnect++;
  client.loop();
}

// task for relay output
void tRelayOutCallback() {
  wind_max(windspeed);
}

// Task for one-Wire temperature
void t_1_wire_measureCallback() {
  // call sensors.requestTemperatures() to issue a global temperature
  sensors.requestTemperatures();
  char* chrBuffer_Temp {""};
  char chrNumber[2];
  float returnTemperature {0.0};

  for (uint8_t i = 0; i < numberTemperatureSensor; i++) {
    returnTemperature = -100.0;
    returnTemperature = read_Temperature(i);
    strcpy(chrBuffer_Temp, strMQTTTopicTemp);
    String strBufferT = String(i);
    strBufferT.toCharArray(chrNumber, 2);
    strcat(chrBuffer_Temp, chrNumber);

    if (temperatures_old[i] != returnTemperature) {
      MQTT_puplish_float (chrBuffer_Temp, returnTemperature);
    }
    temperatures_old[i] = returnTemperature;
  }
}

// task for rain sensor
void tRainSensorCallback() {
  struct Rain_array RainResult;
  // reade rain sensor
  RainResult = check_raining();

  // send with MQTT
  if (RainResult_old.Moisture != RainResult.Moisture ) {
    MQTT_puplish_float (mqtt_topic_rain_Moisture, RainResult.Moisture);
  }

  if (RainResult_old.Result != RainResult.Result ) {
    MQTT_puplish_char(mqtt_topic_rain, RainResult.Result);
  }

  // save state
  RainResult_old = RainResult;
}

void tLEDCallback() {

  if (!client.connected()) {
    ESP8266status = 2;
  }
  else {
    ESP8266status = 1;
  }
  // set LED
  StatusLed(ESP8266status);
}


// *********************************** Setup ****************************************************
void setup() {
  Serial.begin(115200);
  delay(10);
  initStatusLed();

  // We start by connecting to a WiFi network
  initWiFi();

  // set priority, higher is better
  tMQTT.setId(100);
  tLED.setId(50);
  t_1_wire_measure.setId(150);
  tWind.setId(1000);
  tRelayOut.setId(500);
  tRainSensor.setId(160);

  delay(5000);
  initWindsensor();
  initRelays();
  init_rain_sensor();

  runner.setHighPriorityScheduler(&runnerHPR);
  runnerHPR.setHighPriorityScheduler(&runnerCPR);
  runner.enableAll(true); // this will recursively enable the higher priority tasks as well

  client.setServer(mqtt_host, mqtt_port);
  reconnect();
  //  do_update();
  // init One-Wire
  numberTemperatureSensor = Init_temperature();

  // MQTT Topic for number of temperatur sensors
  // send number of Temperaturesensors
  char* MQTTTempNumberTopic {mqtt_topic_temperature_sensors};
  if (debugOutput) {
    Serial.print("MQTT number topic: ");
    Serial.println(MQTTTempNumberTopic);
  }
  MQTT_puplish_float(MQTTTempNumberTopic , numberTemperatureSensor);
}


void loop()
{
  // Task execute
  runner.execute();
  yield();
}
