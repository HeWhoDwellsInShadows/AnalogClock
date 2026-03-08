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