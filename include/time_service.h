#ifndef __TIME_SERVICE__H_
#define __TIME_SERVICE__H_

#include <Arduino.h>

#if defined(ARDUINO_UNOWIFIR4)
    #include <WiFiS3.h>
#elif defined(ARDUINO_PORTENTA_C33)
    #include <WiFiC3.h>
#endif

#include <RTC.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include <TimeLib.h>
#include <Timezone.h>

#include "project_settings.h"
#include "time_zone_list.h"

class time_service
{
public:
    explicit time_service(settings_manager& settings, const char* ntp_server = "pool.ntp.org");

    // Debug stream config setup
    void set_debug_stream(Stream* debug_stream);
    void enable_debug_stream(bool process_verbose);

    // Wifi  & NTP settings
    void set_wifi_credentials(const char* ssid, const char* pass);
    void enaable_ntp_server(bool enable);
    void set_ntp_server(const char* server);
    void set_ntp_resync_period(uint32_t period_ms);
    void set_ntp_retry_period(uint32_t period_ms);
    void wifi_connect_timeout_period(uint32_t period_ms);

    // Network lifecycle functions
    bool begin();
    void update();
    bool force_ntp_synchronisation();

    // Status flags
    bool time_is_valid() const;
    bool time_has_become_valid() const;
    bool ntp_has_just_synched() const;
    bool local_time_is_dst() const;

    // Time variable getters
    time_t utc_time() const;
    time_t local_time() const;

    // Local time translation getters
    int local_time_hours() const;
    int local_time_minutes() const;
    int local_time_seconds() const;

    const char* time_zone_label() const;


    struct config_s
    {
        const char* ntp_pool = "pool.ntp.org";

        bool enable_ntp = true;

        // Wifi connection timing parameters
        uint32_t ntp_resync_period_ms = 6UL * 60UL * 60UL * 1000UL; 
        uint32_t ntp_retry_delay_ms = 30UL * 1000UL;
        uint32_t wifi_connect_timeout_ms = 60UL* 1000UL;

        Stream* debug;
    };

private:
    const char* ssid = nullptr;
    const char* password = nullptr;

    
    
};

#endif