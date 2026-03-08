#include "time_service.h"
#include <stdarg.h>

static constexpr const char* LOG_TAG = "Time";

time_service::time_service(settings_manager& settings, const char* ntp_server)
: _settings(settings),
  _ntp(_udp, ntp_server ? ntp_server : "pool.ntp.org", 0, 60UL * 1000UL),
  _ntp_server(ntp_server ? ntp_server : "pool.ntp.org")
{

}

time_service::~time_service()
{

}

void time_service::set_debug(Stream* debug_stream, bool process_verbose)
{
    set_debug_stream(debug_stream);
    enable_debug_stream(process_verbose);
}

void time_service::set_debug_stream(Stream* debug_stream)
{
    _debug = debug_stream;
}

void time_service::enable_debug_stream(bool process_verbose)
{
    _verbose = process_verbose;
}

void time_service::set_wifi_credentials(const char* ssid, const char* password)
{
    _ssid = ssid;
    _password = password;
}

void time_service::enable_ntp_server(bool enable)
{
    _ntp_enabled = enable;
}

void time_service::set_ntp_server(const char* ntp_server)
{
    if(ntp_server[0] != '\0')
    {
        _ntp_server = ntp_server;

        _ntp.setPoolServerName(_ntp_server);
    }    
}

void time_service::set_ntp_resync_period(uint32_t resync_ms)
{
    _ntp_resync_period_ms = resync_ms;
}

void time_service::set_ntp_retry_period(uint32_t retry_ms)
{
    _ntp_retry_delay_ms = retry_ms;
}

void time_service::set_wifi_connect_timeout_period(uint32_t timeout_ms)
{
    _wifi_connect_timeout_ms = timeout_ms;
}

bool time_service::begin()
{
    RTC.begin();

    //debug_ln("[]")
}

void time_service::debug_ln(const __FlashStringHelper* string) const
{
    if (is_debug_logging_enabled())
        _debug->println(string);
}

void time_service::debug_f(const __FlashStringHelper* format, ...) const
{
    if (is_debug_logging_enabled())
    {
        char buf[180];
        va_list args;
        va_start(args, format);
        vsnprintf(buf, sizeof(buf), (const char*)format, args);
        va_end(args);

        _debug->print(buf);
    }    
}