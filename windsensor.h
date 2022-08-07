/* read Windsensor input and return the wind speed in km/h
   PCE Instruments PCE-WS P
   Author:  Christian Langrock
   Date:    2022-August-06
*/


char* strMQTTTopic_wind {mqtt_topic_wind};
volatile unsigned long CountReed = 0;
float wind  {0.0};
float last_wind  {0.0};
float wind_out {0.0};
//char charBuffer[32];
volatile unsigned long last_micros;
long debouncing_time = 0.1; //in millis
unsigned long cycleTime;
unsigned long cycleTime_old;


// define the interrupt for the wind sensor
void IRAM_ATTR Interrupt()
{
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    CountReed++;
    last_micros = micros();
  }
}

void initWindsensor() {
  pinMode(input_pin_wind, INPUT_PULLUP);
  attachInterrupt(input_pin_wind, Interrupt, RISING);
  cycleTime_old = millis();
}

float windsensor() {

  detachInterrupt(input_pin_wind);
  cycleTime = millis();
  unsigned long runtime = cycleTime - cycleTime_old;

  if (runtime >= 900 && runtime <= 1100) {
    if (CountReed == 0) {
      wind = 0.0;
      wind_out = 0.0;
    }
    else {
      wind = (CountReed * 0.8) + 3; // PCE Instruments PCE-WS P
    }
    wind_out = wind;
    last_wind = wind;
  }
  else {
    wind_out = last_wind;
  }
  CountReed = 0;
  cycleTime_old = cycleTime;

  if (debugOutput) {
    Serial.print("Task Wind running time: ");
    Serial.print(runtime);
    Serial.println ("ms");
  }
 if (debugMeasure) {
    Serial.print("Wind: ");
    Serial.print(wind_out);
    Serial.println(" km/h");
  }
  attachInterrupt(input_pin_wind, Interrupt, RISING);
  return wind_out;
}
