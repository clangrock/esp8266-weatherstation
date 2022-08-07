
// ******************************** OTA Update ************************************
// not ready, see: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266httpUpdate/examples or https://www.iot-embedded.de/iot-2021/beverage-monitoring/over-the-air-updates-ein-simples-beispiel/

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>



void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void do_update(){
  
  WiFiClient espClient; // ???
  if(debugOutput || debugError) Serial.println("do update");
  // auskommentiert  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri, firmware_version);
  // ab hier geändert
  // Add optional callback notifiers
  ESPhttpUpdate.onStart(update_started);
  ESPhttpUpdate.onEnd(update_finished);
  ESPhttpUpdate.onProgress(update_progress);
  ESPhttpUpdate.onError(update_error);
  t_httpUpdate_return ret = ESPhttpUpdate.update(espClient, update_uri, firmware_version);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        if(debugError) Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        if(debugOutput )Serial.println("[update] no Update needed");
        break;
    case HTTP_UPDATE_OK:
        if(debugOutput) Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
