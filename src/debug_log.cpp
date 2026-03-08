#include "debug_log.h"
#include <TimeLib.h>
#include <string.h>

// Global time callback
static log_time_fn_t s_log_time_callback = nullptr;

void log_set_time_callback(log_time_fn_t function)
{
    s_log_time_callback = function;
}

void log_print_time(Stream* stream)
{
    if(stream)
    {
        stream->print('[');

        if(!s_log_time_callback)
        {
            stream->print("--:--:--]");
            return;
        }

        time_t now = s_log_time_callback();

        if(now == 0)
        {
            stream->print("--:--:--]");
            return;
        }

        char buffer[9];
        snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hour(now), minute(now), second(now));
        stream->print(buffer);
        stream->print(']');
    }
}

void log_print_tag(Stream* stream, const char* tag)
{
    if(stream)
    {
        uint8_t len = strlen(tag);

        if (len > LOG_TAG_WIDTH) len = LOG_TAG_WIDTH;

        uint8_t left = (LOG_TAG_WIDTH - len) / 2;
        uint8_t right = LOG_TAG_WIDTH - len - left;

        stream->print('[');

        for (uint8_t i = 0; i < left; i++)
            stream->print(' ');

        for (uint8_t i = 0; i < len; i++)
            stream->print(tag[i]);

        for (uint8_t i = 0; i < right; i++)
            stream->print(' ');

        stream->print(']');
    }    
}

void log_print_level(Stream* stream, log_level_ec level)
{
    if(stream)
    {
        stream->print('[');

        switch(level)
        {
            case log_level_ec::info:
                stream->print("INFO"); 
                break;
            case log_level_ec::warning:
                stream->print("WARN"); 
                break;
            case log_level_ec::error:
                stream->print("ERR "); 
                break;
        }

        stream->print("] ");
    }    
}

void log_printf(Stream* stream, const char* tag, log_level_ec level, const char* format, ...)
{
    if(stream)
    {
        log_print_time(stream);
        log_print_tag(stream, tag);
        log_print_level(stream, level);

        char buffer[120];

        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        stream->print(buffer);
    }    
}

void log_println(Stream* stream, const char* tag, log_level_ec level, const char* message)
{
    if(stream)
    {
        log_print_time(stream);

        log_print_tag(stream, tag);
        log_print_level(stream, level);

        stream->println(message);
    }    
}

const char* log_extract_class(const char* pretty_func)
{
    const char* start = pretty_func;

    // Skip return type if present
    const char* space = strrchr(pretty_func, ' ');
    if (space && *(space + 1) != '\0')
        start = space + 1;

    const char* end = strstr(start, "::");
    if (!end) 
        return start;

    static char name[24];
    size_t len = (size_t)(end - start);
    if (len >= sizeof(name)) len = sizeof(name) - 1;

    memcpy(name, start, len);
    name[len] = '\0';

    return name;
}