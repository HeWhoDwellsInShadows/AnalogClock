#ifndef __PROJECT_SETTINGS__H_
#define __PROJECT_SETTINGS__H_


#include <Arduino.h>
#include <EEPROM.h>

class settings_manager 
{
public:
    // Change version when adding things in settings structure
    static constexpr uint8_t manager_version = 1;

    // Settings option definitions
    enum class temp_unit_ec         : uint8_t {C = 0, F = 1};
    enum class hour_mode_ec         : uint8_t {time_24h = 0, time_12h = 1};
    enum class dst_mode_ec          : uint8_t {auto_mode = 0, force_STD = 1, force_DST = 2};
    enum class needle_mode_ec       : uint8_t {step = 0, smooth_seconds = 1, fully_smooth = 2};
    enum class backlight_mode_ec    : uint8_t {manual = 0, auto_sensor = 1, auto_night = 2, dual_auto = 3};

    struct settings_s
    {
        // Time-related settings
        uint8_t tz_index = 0; // time zone index
        hour_mode_ec hour_mode = hour_mode_ec::time_24h;
        dst_mode_ec dst_mode = dst_mode_ec::auto_mode; // Daylight Saving Time calculation mode

        // Clock behaviour settings
        needle_mode_ec needle_mode = needle_mode_ec::step;

        // Sensors Units
        temp_unit_ec temp_unit = temp_unit_ec::C;

        // LCD settings
        backlight_mode_ec bl_mode = backlight_mode_ec::manual;
        uint8_t manual_brightness           =    80;        // Used in manual and daytime modes (%)
        uint8_t night_brightness            =    20;        // Used at night (%)
        uint8_t auto_min_brightness         =     5;        // Minimal value clamp for Auto mode (%)
        uint8_t auto_max_brightness         =   100;        // Maximal value clamp for Auto mode (%)

        uint16_t photoresistor_adc_dark     =   200;        // Photoresistor reading when considered dark
        uint16_t photoresistor_adc_bright   =   900;        // Photoresistor reading when considered bright

        // Night schedule (local time)
        uint8_t start_night_hour_local      =    22;        // Time at which the lcd brightness dims in auto_night mode
        uint8_t end_night_hour_local        =     6;        // Time at which the lcd brightness brightens in auto_night mode 

        // Global system settings
        uint8_t global_verbose              =     0;        // System-wide verbose toggle
        uint8_t buzzer_enable               =     0;        // Optional buzzer activation toggle

        // Future expansion
        uint8_t reserved[20]                =   {0};
    };

    explicit settings_manager(int base_address = 0);
    ~settings_manager();

    bool load();
    bool save();

    const settings_s& get() const {return _settings;};
    settings_s& edit() {_dirty = true; return _settings;};

    // Convenience setters (optional)
  void setVerbose(bool enable) { _settings.global_verbose = enable ? 1 : 0; _dirty = true; }

private:
    static constexpr uint16_t _mem_magic = 0xFACE;

    struct header_s
    {
        uint16_t magic = _mem_magic;
        uint8_t version = manager_version;
        uint8_t length = sizeof(settings_s);
        uint16_t crc16 = 0;
    };

    bool _dirty = false;
    int _address;
    settings_s _settings{};

    Stream* _debug = nullptr;
    bool _verbose = false;

    bool sanitize();

    static uint16_t crc16_ccitt(const uint8_t* data, size_t length);

    bool check_package_header(const header_s& header) const;
    bool check_package_crc(const header_s& header, const uint8_t* data, size_t size) const;
    void repair_settings();

    void debug_print_log(const char* message) const;
};  

#endif