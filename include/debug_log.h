#ifndef __DEBUG_LOG__H_
#define __DEBUG_LOG__H_

#include <Arduino.h>
#include <stdarg.h>

constexpr uint8_t LOG_TAG_WIDTH = 10;

// Log levels
enum class log_level_ec : uint8_t
{
    info,
    warning,
    error
};

// Time callback type
// Should return local time as unix time if valid, otherwise 0.
using log_time_fn_t = time_t (*)();

// Global logger configuration 
void log_set_time_callback(log_time_fn_t function);

// Print log helpers
void log_print_time(Stream* stream);
void log_print_tag(Stream* stream, const char* tag);
void log_print_level(Stream* stream, log_level_ec level);

// Main logging functions
// Formatted logging
void log_printf(Stream* stream, const char* tag, log_level_ec level, const char* format, ...);
// Simple line logging
void log_println(Stream* stream, const char* tag, log_level_ec level, const char* message);

// Class tag detection helper
const char* log_extract_class(const char* pretty);

// Macros
#define LOG_CLASS_TAG log_extract_class(__PRETTY_FUNCTION__);
#define LOG_INFO(stream, fmt, ...) log_printf(stream, LOG_CLASS_TAG, log_level_ec::info,  fmt, ##__VA_ARGS__)
#define LOG_WARN(stream, fmt, ...) log_printf(stream, LOG_CLASS_TAG, log_level_ec::warning,  fmt, ##__VA_ARGS__)
#define LOG_ERR(stream, fmt, ...)  log_printf(stream, LOG_CLASS_TAG, log_level_ec::error,  fmt, ##__VA_ARGS__)

#endif


/*
#include "debug_log.h"
#include <TimeLib.h>
#include <string.h>

// Global time callback
static log_time_fn_t s_log_time_callback = nullptr;

void log_set_time_callback(log_time_fn_t fn)
{
    s_log_time_callback = fn;
}

void log_print_time(Stream* s)
{
    if (!s) return;

    s->print('[');

    if (!s_log_time_callback)
    {
        s->print("--:--:--");
        s->print(']');
        return;
    }

    time_t now = s_log_time_callback();
    if (now == 0)
    {
        s->print("--:--:--");
        s->print(']');
        return;
    }

    char buf[9];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour(now), minute(now), second(now));
    s->print(buf);
    s->print(']');
}

void log_print_tag(Stream* s, const char* tag)
{
    if (!s) return;

    uint8_t len = strlen(tag);
    if (len > LOG_TAG_WIDTH) len = LOG_TAG_WIDTH;

    uint8_t left = (LOG_TAG_WIDTH - len) / 2;
    uint8_t right = LOG_TAG_WIDTH - len - left;

    s->print('[');

    for (uint8_t i = 0; i < left; i++)
        s->print(' ');

    for (uint8_t i = 0; i < len; i++)
        s->print(tag[i]);

    for (uint8_t i = 0; i < right; i++)
        s->print(' ');

    s->print(']');
}

void log_print_level(Stream* s, log_level_e level)
{
    if (!s) return;

    s->print('[');

    switch (level)
    {
        case log_level_e::INFO:  s->print("INFO "); break;
        case log_level_e::WARN:  s->print("WARN "); break;
        case log_level_e::ERROR: s->print("ERROR"); break;
    }

    s->print(']');
    s->print(' ');
}

void log_printf(Stream* s, const char* tag, log_level_e level, const char* fmt, ...)
{
    if (!s) return;

    log_print_time(s);
    log_print_tag(s, tag);
    log_print_level(s, level);

    char buffer[180];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    s->print(buffer);
}

void log_println(Stream* s, const char* tag, log_level_e level, const char* msg)
{
    if (!s) return;

    log_print_time(s);
    log_print_tag(s, tag);
    log_print_level(s, level);
    s->println(msg);
}

const char* log_extract_class(const char* pretty)
{
    // Example __PRETTY_FUNCTION__:
    // "void time_service::update()"
    // We want "time_service"

    const char* start = pretty;

    // Skip return type if present
    const char* space = strrchr(pretty, ' ');
    if (space && *(space + 1) != '\0')
        start = space + 1;

    const char* end = strstr(start, "::");
    if (!end) return start;

    static char name[24];
    size_t len = (size_t)(end - start);
    if (len >= sizeof(name)) len = sizeof(name) - 1;

    memcpy(name, start, len);
    name[len] = '\0';

    return name;
}

*/
