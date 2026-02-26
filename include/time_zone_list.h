#ifndef __TIME_ZONES__H_
#define __TIME_ZONES__H_

#include <Arduino.h>

struct time_zone_item_s
{
    const char* lcd_label;
    const char* zone_name; //IANA zone name
};

static const time_zone_item_s time_zones[] = 
{
    // ===== Canada =====
  { "Montreal",        "America/Toronto" },      // 🇨🇦 Default
  { "Vancouver",       "America/Vancouver" },
  { "Calgary",         "America/Edmonton" },
  { "Winnipeg",        "America/Winnipeg" },
  { "Halifax",         "America/Halifax" },
  { "St. Johns",       "America/St_Johns" },     // half-hour zone

  // ===== United States =====
  { "New York",        "America/New_York" },
  { "Chicago",         "America/Chicago" },
  { "Denver",          "America/Denver" },
  { "Phoenix",         "America/Phoenix" },      // no DST
  { "Los Angeles",     "America/Los_Angeles" },
  { "Anchorage",       "America/Anchorage" },
  { "Honolulu",        "Pacific/Honolulu" },     // no DST

  // ===== Europe =====
  { "London",          "Europe/London" },
  { "Paris",           "Europe/Paris" },
  { "Berlin",          "Europe/Berlin" },
  { "Rome",            "Europe/Rome" },
  { "Madrid",          "Europe/Madrid" },
  { "Warsaw",          "Europe/Warsaw" },
  { "Athens",          "Europe/Athens" },

  // ===== Asia =====
  { "Dubai",           "Asia/Dubai" },           // no DST
  { "Mumbai",          "Asia/Kolkata" },         // half-hour zone
  { "Bangkok",         "Asia/Bangkok" },
  { "Singapore",       "Asia/Singapore" },
  { "Shanghai",        "Asia/Shanghai" },
  { "Seoul",           "Asia/Seoul" },
  { "Tokyo",           "Asia/Tokyo" },

  // ===== Australia / Pacific =====
  { "Sydney",          "Australia/Sydney" },
  { "Adelaide",        "Australia/Adelaide" },   // half-hour
  { "Perth",           "Australia/Perth" },
  { "Auckland",        "Pacific/Auckland" },

  // ===== Reference =====
  { "UTC",             "Etc/UTC" }
};

// Total number of available time zones
static constexpr uint8_t time_zones_count =
    (uint8_t)(sizeof(time_zones) / sizeof(time_zones[0]));

// Default time zone setting
static constexpr uint8_t default_time_zone_index = 0; // Montreal

#endif