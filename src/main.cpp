#include <Arduino.h>
#include "debug_log.h"
#include "time_service.h"
#include "project_settings.h"
#include "secrets.h"
#include "keypad.h"

settings_manager settings(0);
time_service ts(settings);

keypad kp(A0,keypad::decode_mode_ec::closest_match, 20, 30, false);

// callback used by logger
time_t logger_time_callback()
{
    if (!ts.time_is_valid())
        return 0;

    return ts.local_time();
}

void setup() 
{
    // Init serial debug output
    Serial.begin(115200);

    pinMode(10,OUTPUT);
    digitalWrite(10,HIGH);

    settings.load();

    ts.set_debug(&Serial, true);
    ts.set_wifi_credentials(secrets::WIFI_SSID, secrets::WIFI_PASS);
    ts.begin();

    // connect logger to current local time
    //log_set_time_callback(logger_time_callback);

    //LOGI(&Serial, "System startup complete\n");

//    test_struct = {.a = 0xFACE, .b = 0xCAFE, .c = 0xDEADBEEF, .buffer = "Hello world again! test"};
}

void loop() 
{
    kp.update();
    //ts.update();

    if (kp.changed()) {
        Serial.println("Key change detected!");
        kp.debug_print_key();
        Serial.println();
    }    
    
/*
    if (ts.ntp_has_just_synched())
    {
        //LOGI(&Serial, "NTP synchronized\n");
    }*/
        
}

