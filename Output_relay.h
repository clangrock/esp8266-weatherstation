/* Output with relay
 * 
 * Author:  Christian Langrock
 * Date:    2022-July-31
 */

int timeMaxWindSpeed = 0;
long timeMaxMaxWindSpeed = 0;
bool relay = false;

void wind_max(int wind){
  // task runs every 2s!

  // decrase timer for max max wind speed
  if (timeMaxMaxWindSpeed >= 1){
      timeMaxMaxWindSpeed = timeMaxMaxWindSpeed - 2;
  }

  if (wind >= Config_max_max_wind_speed) {
      relay = true;
      timeMaxMaxWindSpeed = Config_timeMaxMaxWindSpeed;
    }
    else if (wind >= Config_max_wind_speed){
      if (timeMaxWindSpeed <= Config_timeMaxWindSpeed){
        timeMaxWindSpeed = timeMaxWindSpeed + 2;
      }
    }
   else if (wind <= (Config_max_wind_speed - Config_wind_hyst)&& (timeMaxMaxWindSpeed <= 0)){
     if (timeMaxWindSpeed >= 4){
        timeMaxWindSpeed = timeMaxWindSpeed - 4;
     }
     else if(timeMaxWindSpeed == 2){
        timeMaxWindSpeed = timeMaxWindSpeed - 2;
     }
  }
  
  if (timeMaxWindSpeed > Config_timeMaxWindSpeed){
    relay = true;
  }
  else if (timeMaxWindSpeed <= 0){
    relay = false;
  }
  // toggle relays
    if (relay) {
      digitalWrite(output_pin_Relay_1, LOW);
      digitalWrite(output_pin_Relay_2, LOW);
    }
    else {
      digitalWrite(output_pin_Relay_1, HIGH);
      digitalWrite(output_pin_Relay_2, HIGH);
    }
    if(debugOutput){
        Serial.print("time Max wind: ");
        Serial.println(timeMaxWindSpeed);
        Serial.print("time Max Max wind: ");
        Serial.println(timeMaxMaxWindSpeed);
     }
}
