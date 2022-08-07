
/* Weatherstation
   rain sensor
   Author:  Christian Langrock
   Date:    2022-August-06
*/

char* strMQTTTopicRain {mqtt_topic_rain};

void init_rain_sensor() {

  if (debugOutput) {
    Serial.println("Rain sensor");
  }
  pinMode(DIGITAL_PIN_RAIN, INPUT);

  // build MQTT topic
  if (debugOutput) {
    Serial.print("MQTT topic prefix for rainning: ");
    Serial.println(strMQTTTopicRain);
  }
}


struct Rain_array check_raining() {
  struct Rain_array OutArray;
  String raining;
  float rainVal = analogRead(ANALOG_PIN);
  bool isRaining = digitalRead(DIGITAL_PIN_RAIN);

  if (isRaining) {
    raining = "No";
    OutArray.Result = "False";
  }
  else {
    raining = "Yes";
    OutArray.Result = "True";
  }

  rainVal = map(rainVal, 0, 1023, 100, 0);
  OutArray.Moisture = rainVal;
  if (debugMeasure) {
    Serial.print("Raining: ");
    Serial.println(raining);
    Serial.print("Moisture: ");
    Serial.print(rainVal);
     Serial.println("%");
  }
  return OutArray;
}
