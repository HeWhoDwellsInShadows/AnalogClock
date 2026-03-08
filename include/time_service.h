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

#include "debug_log.h"

class time_service
{
public:
    explicit time_service(settings_manager& settings, const char* ntp_server = "pool.ntp.org");
    ~time_service();

    // Debug stream config setup
    void set_debug(Stream* debug_stream, bool process_verbose);
    void set_debug_stream(Stream* debug_stream);
    void enable_debug_stream(bool process_verbose);

    // Wifi  & NTP settings
    void set_wifi_credentials(const char* ssid, const char* password);
    void enable_ntp_server(bool enable);
    void set_ntp_server(const char* server);
    void set_ntp_resync_period(uint32_t resync_ms);
    void set_ntp_retry_period(uint32_t retry_ms);
    void set_wifi_connect_timeout_period(uint32_t timeout_ms);

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

private:
    const time_zone_item_s& time_zone() const;
    bool ensure_wifi_connection(uint32_t wifi_timeout_ms);
    bool synch_time_from_ntp();
    bool rtc_time_looks_valid();

    time_t apply_time_zone_to_local(time_t utc_time) const;
    time_t apply_dst_override(time_t utc_time, time_t local_time, bool enable_dst);

    bool is_debug_logging_enabled() const;
    void debug_ln(const __FlashStringHelper* string) const;
    void debug_f(const __FlashStringHelper* format, ...) const;

    settings_manager& _settings;

    const char* _ssid = nullptr;
    const char* _password = nullptr;

    WiFiUDP _udp;
    NTPClient _ntp;

    const char* _ntp_server = "pool.ntp.org";
    bool _ntp_enabled = true;
    
    // Wifi connection timing parameters
    uint32_t _ntp_resync_period_ms = 6UL * 60UL * 60UL * 1000UL; 
    uint32_t _ntp_retry_delay_ms = 30UL * 1000UL;
    uint32_t _wifi_connect_timeout_ms = 60UL* 1000UL;

    uint32_t _last_attempt_ms = 0;
    uint32_t _last_success_ms = 0;

    Stream* _debug = nullptr;
    bool _verbose = false;
};

#endif