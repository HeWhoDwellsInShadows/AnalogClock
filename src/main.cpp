#include <Arduino.h>
#include "keypad.h"

Keypad kp(A0,Keypad::Decode_mode::closest_match, 10, 25);

void setup() 
{
    // Init serial debug output
    Serial.begin(115200);

}

void loop() 
{
    kp.update();

  if (kp.changed()) {
    // react to kp.key()
  }
}

