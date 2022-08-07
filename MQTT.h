
/* MQTT client functions
   Author:  Christian Langrock
   Date:    2022-August-07
*/

#include <PubSubClient.h>

PubSubClient client(espClient);

//long lastReconnectAttempt = 0;

// connect to MQTT Broker
boolean reconnect() {
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
      if(debugError){
        Serial.print("MQTT connection failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  }
  return client.connected();
}

bool MQTT_puplish_float (char* strMQTTTopic, float fMQTTValue) {
      bool Com_Ok  {false};
      char charBuffer[32];
      String strBuffer;
      strBuffer =  String(fMQTTValue);
      strBuffer.toCharArray(charBuffer,10);
      
      if (!client.publish(strMQTTTopic, charBuffer, false)){
        if(debugError) {
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

bool MQTT_puplish_char (char* strMQTTTopic, char* fMQTTValue) {
      bool Com_Ok  {false};
      
      if (!client.publish(strMQTTTopic, fMQTTValue, false)){
        if(debugError) {
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
