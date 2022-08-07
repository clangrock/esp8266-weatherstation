
/* Weatherstation
   builtin LED as status
   Author:  Christian Langrock
   Date:    2022-August-07
*/

int countLED {0};

void initStatusLed() {
  pinMode(LED_BUILTIN_1, OUTPUT);
  //LED on
  ESP8266status = 0;
}

void StatusLed(int ESPstatus) {

  switch (ESPstatus) {
    case 0:
      //LED on
      digitalWrite(LED_BUILTIN_1, LOW);
      break;
    case 1:
      // normal run, slow blink
      if ((digitalRead(LED_BUILTIN_1) == HIGH) && (countLED == 4)) {
        digitalWrite(LED_BUILTIN_1, LOW);
        countLED = 0;
      }
      else if ((digitalRead(LED_BUILTIN_1) == LOW) && (countLED == 4))  {
        digitalWrite(LED_BUILTIN_1, HIGH);
        countLED = 0;
      }
      break;
    case 2:
      // run with error, fast blink
      if (digitalRead(LED_BUILTIN_1) == HIGH) {
        digitalWrite(LED_BUILTIN_1, LOW);
        countLED = 0;
      }
      else {
        digitalWrite(LED_BUILTIN_1, HIGH);
      }
      break;
    default:
      digitalWrite(LED_BUILTIN_1, LOW);
      break; // Wird nicht benötigt, wenn Statement(s) vorhanden sind
  }
  countLED++;
}
