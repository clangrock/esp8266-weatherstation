/* read Windsensor input and return the wind speed in km/h
 * PCE Instruments PCE-WS P 
 * Author:  Christian Langrock
 * Date:    2022-July-31
 */


char* strMQTTTopic_wind;
char* chrSubTopic {"/wind"};
int count  {0};
volatile unsigned long CountReed = 0;
float wind  {0.0};
float last_wind  {0.0};
float wind_out {0.0};
//char charBuffer[32];
volatile unsigned long last_micros;
long debouncing_time = 0.1; //in millis


// init MQTT topic
void initWindsensor(){  
  strMQTTTopic_wind = mqtt_topic_prefix;
  strcpy(strMQTTTopic_wind, mqtt_topic_prefix);
  chrSubTopic = "/wind";
  strcat(strMQTTTopic_wind, chrSubTopic);
  if(debugOutput){
    Serial.print("Prefix MQTT wind: ");
    Serial.println(strMQTTTopic_wind);
  }
}

void IRAM_ATTR Interrupt()
{
  if((long)(micros() - last_micros) >= debouncing_time * 1000) {
    CountReed++;
    last_micros = micros();
  }
}


float windsensor(){

  detachInterrupt(input_pin_wind);
    count++; 
    if(CountReed == 0){
      wind = 0.0;
      wind_out = 0.0;
    }
    else{
            wind = (CountReed * 0.8) + 3; // PCE Instruments PCE-WS P 
    }
    if(debugOutput){
      Serial.print("Impulse: ");
      Serial.print(CountReed);
      Serial.println(" Imp");
    }
    if(last_wind - wind > 0.8 || last_wind - wind < -0.8 || count >= 10){
      if(debugOutput){
        Serial.print("Wind: ");
        Serial.print(wind);
        Serial.println(" km/h");
      }
      wind_out = wind;
      count = 0;
    }
    CountReed = 0;
    last_wind = wind;

    attachInterrupt(input_pin_wind,Interrupt,RISING);

    return wind_out;
}
