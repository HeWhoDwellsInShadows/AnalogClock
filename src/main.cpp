#include <Arduino.h>
#include "keypad.h"

#include "SPI.h"

keypad kp(A0,keypad::decode_mode_ec::closest_match, 20, 30, false);

void setup() 
{
    // Init serial debug output
    Serial.begin(115200);

    pinMode(10,OUTPUT);
    digitalWrite(10,HIGH);
}

void loop() 
{
    kp.update();

    if (kp.changed()) {
        Serial.println("Key change detected!");
        kp.debug_print_key();
        Serial.println();
    }
}

