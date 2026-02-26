#include "project_settings.h"
#include "time_zone_list.h"

settings_manager::settings_manager(int base_address) : _address(base_address)
{

}

settings_manager::~settings_manager()
{

}

uint16_t settings_manager::crc16_ccitt(const uint8_t* data, size_t length)
{
    uint16_t crc = 0xFFFF;

    for(size_t index = 0; index < length; index++)
    {
        crc ^= (uint16_t)data[index] << 8;

        for(uint8_t _bit = 0; _bit < 8; _bit++)
        {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x1021) : (uint16_t)(crc << 1);
        }
    }

    return crc;
}

bool settings_manager::check_package_header(const header_s& header) const
{
    if(header.magic != _mem_magic) 
        return false;
    if(header.version != manager_version)
        return false;
    if(header.length != sizeof(settings_s))
        return false;
    
    return true;
}

bool settings_manager::check_package_crc(const header_s& header, const uint8_t* data, size_t size) const
{
    if (!data || size == 0)
        return false;

    return crc16_ccitt(data, size) == header.crc16;
}

bool settings_manager::load()
{
    debug_print_log("Loading settings from memory...");

    // Read stored settings header
    header_s header{};
    EEPROM.get(_address, header);

    // If currently stored settings header is problematic, go to defaults and rewrite
    if(!check_package_header(header))
    {
        debug_print_log("Settings header has been found to be problematic");
        repair_settings();
        return false; // Indicate rewrite
    }

    // Read settings payload
    settings_s temp{};
    EEPROM.get(_address + (int)sizeof(header_s), temp);
    const uint8_t* data = reinterpret_cast<const uint8_t*>(&temp);

    // If currently stored settings payload is problematic, go to defaults and rewrite
    if(!check_package_crc(header, data, sizeof(settings_s)))
    {
        debug_print_log("Settings payload has been found to be problematic");
        repair_settings();
        return false; // Indicate rewrite
    }

    // Register current stored settings to parameters
    _settings = temp;

    if(sanitize())
    {
        debug_print_log("Settings have been sanitized while loading");
        _dirty = true;
        save();
        return false;
    }

    debug_print_log("Settings have been loaded from internal memory");
    
    return true;
}

bool settings_manager::save()
{
    debug_print_log("Saving settings to memory...");

    if(!_dirty)
    {
        debug_print_log("No changes found, cancelling...");
        return true;
    }
        

    if (sanitize()) 
    {
        debug_print_log("Settings have been sanitized while saving");
    }

    header_s header{};    

    header.magic = _mem_magic;
    header.version = manager_version;
    header.length = sizeof(settings_s);

    const uint8_t* data = reinterpret_cast<const uint8_t*>(&_settings);
    header.crc16 = crc16_ccitt(data, header.length);

    EEPROM.put(_address, header);
    EEPROM.put(_address + (int)sizeof(header_s), _settings);

    debug_print_log("Settings have been saved to internal flash memory");

    _dirty = false;
    return true;    
}

bool settings_manager::sanitize()
{
    bool settings_changed = false;

    if(_settings.tz_index >= time_zones_count)
    {
        _settings.tz_index = default_time_zone_index;
        settings_changed = true;
    }

    if(static_cast<uint8_t>(_settings.hour_mode) > 1)
    {
        _settings.hour_mode = hour_mode_ec::time_24h;
        settings_changed = true;
    }

    if (static_cast<uint8_t>(_settings.dst_mode) > 2)
    {
        _settings.dst_mode = dst_mode_ec::auto_mode;
        settings_changed = true;
    }

    if (static_cast<uint8_t>(_settings.needle_mode) > 2)
    {
        _settings.needle_mode = needle_mode_ec::step;
        settings_changed = true;
    }

    if (static_cast<uint8_t>(_settings.bl_mode) > 3)
    {
        _settings.bl_mode = backlight_mode_ec::manual;
        settings_changed = true;
    }

    if (static_cast<uint8_t>(_settings.temp_unit) > 1)
    {
        _settings.temp_unit = temp_unit_ec::C;
        settings_changed = true;
    }

    // Value clamping lambda helper function
    auto clamp_u8 = [](uint8_t& value, uint8_t min, uint8_t max)
    {
        if (value < min) { value = min; return true; }
        if (value > max) { value = max; return true; }
        return false;
    };

    settings_changed |= clamp_u8(_settings.manual_brightness, 0, 100);
    settings_changed |= clamp_u8(_settings.night_brightness, 0, 100);
    settings_changed |= clamp_u8(_settings.auto_min_brightness, 0, 100);
    settings_changed |= clamp_u8(_settings.auto_max_brightness, 0, 100);

    if (_settings.auto_min_brightness > _settings.auto_max_brightness)
    {
        _settings.auto_min_brightness = 5;
        _settings.auto_max_brightness = 100;
        settings_changed = true;
    }    

    if (_settings.start_night_hour_local > 23)
    {
        _settings.start_night_hour_local = 22;
        settings_changed = true;
    }

    if (_settings.end_night_hour_local > 23)
    {
        _settings.end_night_hour_local = 6;
        settings_changed = true;
    }

    if (_settings.photoresistor_adc_dark >= _settings.photoresistor_adc_bright)
    {
        _settings.photoresistor_adc_dark = 200;
        _settings.photoresistor_adc_bright = 900;
        settings_changed = true;
    }

    if (_settings.global_verbose > 1)
    {
        _settings.global_verbose = 0;
        settings_changed = true;
    }

    if (_settings.buzzer_enable > 1)
    {
        _settings.buzzer_enable = 0;
        settings_changed = true;
    }

    return settings_changed;
}

void settings_manager::repair_settings()
{
    _settings = settings_s{};
    _dirty = true;

    debug_print_log("Settings memory is been repaired");

    save();
}

void settings_manager::debug_print_log(const char* message) const
{
    if(_verbose && _debug)
    {
        _debug->println(message);
    }
}