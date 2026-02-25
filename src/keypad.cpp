#include "keypad.h"

constexpr Keypad::Key_entry Keypad::key_lookup[];

Keypad::Keypad(uint8_t analogPin, 
               Decode_mode mode,
               uint16_t tolerance,
               uint16_t debounce_ms,
               bool process_verbose,
               Stream* debug_stream):
               _pin(analogPin),
               _mode(mode),
               _tolerance(tolerance),
               _debounce_ms(debounce_ms),
               _verbose(process_verbose),
               _debug(debug_stream)
{
    build_thresholds_table();

    // This class assumes the use of an ADC with a 14-bit resolution to work
    analogReadResolution(KEYPAD_ADC_RESOLUTION);
}

Keypad::~Keypad()
{

}

void Keypad::set_verbose(bool process_verbose) 
{
    _verbose = process_verbose;
}

void Keypad::set_debug_stream(Stream* debug_stream)
{
    _debug = debug_stream;
}

void Keypad::set_tolerance(uint16_t tolerance)
{
    _tolerance = tolerance;
}

void Keypad::set_debounce_ms(uint16_t debounce_ms)
{
    _debounce_ms = debounce_ms;
}

void Keypad::set_decode_mode(Decode_mode mode)
{
    _mode = mode;
}

int Keypad::raw() const
{
    return _raw_value;
}

Keypad::Decode_mode Keypad::mode() const
{
    return _mode;
}

Keypad::Key Keypad::key() const
{
    return _stable_key;
}

Keypad::Key Keypad::last_key() const
{
    return _last_stable;
}

bool Keypad::changed() const
{
    return _stable_key != _last_stable;
}

void Keypad::build_thresholds_table()
{
    // Calculate the midpoint between each adc center value
    for(size_t index = 0; index < lookup_size - 1; index++)
    {
        uint32_t val_a = key_lookup[index].adc_value;
        uint32_t val_b = key_lookup[index + 1].adc_value;

        _thresholds[index] = (uint16_t)(val_a + val_b) / 2u;
    }
}

void Keypad::update()
{
    _raw_value = analogRead(_pin);    

    // Defensive raw value clamping
    if(_raw_value < 0)
        _raw_value = 0;
    if(_raw_value > adc_max_value)
        _raw_value = adc_max_value;

    if(_verbose)
    {
        _debug->print("Analog Keypad Raw Value: ");
        _debug->println(_raw_value);
    }

    Key decoded = decode((uint16_t)_raw_value);

    // Software key debouncing : switch to stable only after same key for debounce time
    uint32_t now = millis();

    if(decoded != _candidate_key)
    {
        _candidate_key = decoded;
        _candidate_mark = now;
    }

    if((now - _candidate_mark) >= _debounce_ms)
    {
        _last_stable = _stable_key;
        _stable_key = _candidate_key;

        if(_verbose && _stable_key != _last_stable)
        {
            debug_print_key();
        }
    }
}

Keypad::Key Keypad::decode(uint16_t raw_value) const
{
    switch(_mode)
    {
        case Decode_mode::closest_match:
            return decode_closest_match(raw_value);
        case Decode_mode::thresholds:
            return decode_thresholds(raw_value);
        default:
            return Key::error;
    }
}

Keypad::Key Keypad::decode_closest_match(uint16_t raw_value) const
{
    uint16_t best_value_distance = 0xFFFF;
    Key best_decoded_key = Key::error;

    // Find closest value in the lookup table
    for(size_t index = 0; index < lookup_size; index++)
    {
        const Key_entry &entry = key_lookup[index];
        uint16_t distance = raw_value > entry.adc_value ? (raw_value - entry.adc_value) : 
                                                          (entry.adc_value - raw_value);

        if(distance < best_value_distance)
        {
            best_value_distance = distance;
            best_decoded_key = entry.key;
        }
    }

    // Return key if within tolerances of that key
    if(best_value_distance <= _tolerance)
        return best_decoded_key;

    // If value not in tolerances but between adjacent centers,
    // guess that it is ambiguous and multiple keys are pressed.
    for(size_t index = 0; index < lookup_size - 1; index++)
    {
        uint16_t low_value = key_lookup[index].adc_value;
        uint16_t high_value = key_lookup[index + 1].adc_value;

        if(raw_value > low_value && raw_value < high_value)
            return Key::multiple;
    }

    return Key::error;
}

Keypad::Key Keypad::decode_thresholds(uint16_t raw_value) const
{
    // Search through threshold table until raw value is under entry
    for(size_t index = 0; index < lookup_size - 1; index++)
    {
        if(raw_value < _thresholds[index])
        {
            return key_lookup[index].key;
        }
    }

    // If entry not found, NONE is returned
    return key_lookup[lookup_size - 1].key;
}

void Keypad::debug_key(Key key_to_print)
{
    switch(key_to_print)
    {
        case Key::none:
            _debug->print("NONE");
            break;
        case Key::select:
            _debug->print("SELECT");
            break;
        case Key::left:
            _debug->print("LEFT");
            break;
        case Key::up:
            _debug->print("UP");
            break;
        case Key::down:
            _debug->print("DOWN");
            break;
        case Key::right:
            _debug->print("RIGHT");
            break;
        case Key::multiple:
            _debug->print("MULTIPLE");
            break;
        default:
            _debug->print("ERROR");
            break;
    }
}

void Keypad::debug_print_key()
{
    _debug->print("\tCurrent key pressed: ");
    debug_key(_stable_key);
    _debug->print(" (last key: ");
    debug_key(_last_stable);
    _debug->println(")");
}