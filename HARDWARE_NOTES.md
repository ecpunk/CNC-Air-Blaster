# Hardware Notes & Lessons Learned

## Overview
This document captures hardware-specific insights, quirks, and lessons learned during development of the CNC Air Blaster controller. **Use this as a reference for future ESP32-based projects** with similar components (encoders, OLED displays, NeoPixels, etc.).

## Components

### MCU: ESP32-WROOM-32
- **Operating Voltage**: 3.3V logic (5V tolerant on some pins—check datasheet!)
- **Flash**: 4MB
- **RAM**: 320KB SRAM
- **Clock**: 240MHz dual-core (Xtensa LX6)
- **GPIO**: 34 pins (but many have restrictions)
- **Communication**: I2C, SPI, UART, CAN, I2S
- **ADC**: 18 channels, 12-bit (note: nonlinear, needs calibration for precision)
- **Timers**: 4 × 64-bit general-purpose timers
- **PWM**: 16 channels
- **Built-in**: WiFi, Bluetooth Classic, BLE
- **Upload/Monitor speeds**:
  - Upload: 921600 baud (fast development; reduce if uploads fail)
  - Serial monitor: 115200 baud (standard)
- **Pin restrictions** (important!):
  - **Strapping pins** (avoid or use carefully):
    - GPIO 0: Boot mode (pulled high for normal boot)
    - GPIO 2: Boot mode / UART debug
    - GPIO 5: Boot mode
    - GPIO 12: Flash voltage (MTDI)
    - GPIO 15: Debug output / boot timing (MTDO)
  - **Input-only pins**: GPIO 34-39 (no pull-up/down, ADC only)
  - **I2C default**: GPIO 21 (SDA), GPIO 22 (SCL) but any GPIO works
  - **SPI default**: GPIO 18 (SCK), 19 (MISO), 23 (MOSI), 5 (CS)
  - **Reserved/unavailable**: GPIO 6-11 (connected to flash)
- **Safe GPIO choices for general use**:
  - GPIO 4, 13, 14, 16, 17, 25, 26, 27, 32, 33
- **Power**:
  - USB or VIN (5V regulated to 3.3V on-board)
  - 3.3V pin: Max ~500mA depending on regulator
  - Deep sleep current: ~10µA (excellent for battery projects)
- **Development tips**:
  - Hold BOOT button while uploading if auto-reset fails
  - Use `Serial.begin(115200)` in setup for debugging
  - Watch for brownout resets if drawing too much current
- **Common issues**:
  - **Boot loop**: Check strapping pins aren't pulled incorrectly
  - **Upload fails**: Reduce upload speed, hold BOOT button, check USB cable/driver
  - **GPIO conflicts**: Review pinout carefully; avoid strapping pins for critical I/O
- **Lessons learned**:
  - GPIO 21/22 for I2C "just works" (default Wire pins)
  - Higher upload speeds (921600) save time but may fail on poor USB cables
  - Always test GPIO pin availability before assigning in schematic

### Display: 0.96" OLED SSD1306 (I2C)
- **Resolution**: 128x64 pixels
- **Interface**: I2C
  - SDA: GPIO 21
  - SCL: GPIO 22
  - VCC: 3.3V
  - GND: Ground
- **I2C Address**: 0x3C (default, fixed on most modules)
- **Library**: Adafruit SSD1306 v2.5.9+ and Adafruit GFX Library v1.11.10+
- **Text sizing**:
  - Size 1: 6×8 pixels per character → ~21 chars/line, 8 lines
  - Size 2: 12×16 pixels per character → ~10 chars/line, 4 lines
  - Size 3: 18×24 pixels per character → ~7 chars/line, 2-3 lines
  - **Recommendation**: Use size 1 for most text; size 2 sparingly for emphasis
- **Layout tips for 128×64**:
  - Keep labels short (e.g., "LED" instead of "LED Brightness")
  - Use inversion/highlighting instead of larger text for selection
  - Test actual pixel width: `strlen(text) × (6 × size)` must be ≤ 128
  - Leave margin—text at x=128 will wrap/clip
- **Brightness/contrast**:
  - Adjustable via `ssd1306_command(SSD1306_SETCONTRAST)` + value (0-255)
  - Default (0x7F/127) is usually fine
  - Higher values = brighter but more power consumption
- **Performance**:
  - Use hash-based change detection to avoid unnecessary redraws
  - `display.clearDisplay()` before each screen update
  - Minimize `display.display()` calls (only when content changes)
- **Common issues**:
  - **Display not initializing**: Check I2C address (try 0x3D if 0x3C fails), verify wiring
  - **Text overflow/wrapping**: Calculate pixel width before printing
  - **Flicker**: Minimize display() calls; use dirty-flag pattern
- **Lessons learned**:
  - Size 2 text for "LED Brightness" = 14 chars × 12px = 168px → doesn't fit on 128px screen!
  - Always calculate and test text widths during layout design
  - Inversion (white-on-black vs black-on-white) is very effective for highlighting

### Input: KY-040 Rotary Encoder with Push Button
- **Pins**: 
  - CLK (A): GPIO 25
  - DT (B): GPIO 26
  - SW (Button): GPIO 27
  - VCC: 3.3V (not 5V!)
  - GND: Ground
- **Pull resistors**: 
  - Module has built-in 10kΩ pull-ups on CLK/DT
  - SW pin needs external pull-down OR use INPUT_PULLUP mode (auto-detected in code)
- **Quadrature decoding**:
  - 4 state transitions per mechanical detent (click)
  - Gray code sequence: 00 → 01 → 11 → 10 → 00 (CW) or reverse (CCW)
  - Must accumulate 4 transitions before registering a logical "step"
  - **Critical**: Filter illegal transitions to prevent jitter/bounce
- **Debounce timing**:
  - Minimum event spacing: 5ms for stable readings
  - Encoder events: 5ms minimum between transitions
  - Button events: 50ms debounce window
- **Decoding table** (see `encoder_handler.cpp`):
  ```
  [prev_state][new_state] → delta
  Illegal transitions return 0 and are ignored
  ```
- **Common issues**:
  - Encoder "double-stepping" → Reduce sensitivity or increase transition count
  - Direction reversed → Swap CLK/DT pins or invert delta in code
  - Missed steps → Check for EMI/noise, add filtering
- **Lessons learned**:
  - Always use a state machine with illegal transition rejection
  - Don't trust every edge—accumulate transitions before acting
  - Test CW/CCW behavior early in development

### LED: WS2812 RGB (NeoPixel)
- **Pin**: GPIO 5 (any GPIO will work, but avoid strapping pins)
- **Protocol**: Single-wire serial (800kHz bitstream)
- **Voltage**: 5V preferred (works at 3.3V with reduced brightness/range)
- **Current**: ~60mA per LED at full white brightness
- **Library**: Adafruit NeoPixel v1.11.3+
- **Configuration**: `NEO_GRB + NEO_KHZ800`
  - NEO_GRB: Color order (Green-Red-Blue)
  - NEO_KHZ800: 800kHz data rate
- **Brightness control**:
  - Global: `setBrightness(0-255)` before `show()`
  - Per-pixel: `setPixelColor(n, r, g, b)` with scaled RGB values
  - User-adjustable: 10-100% (mapped to 26-255 internally for visibility)
- **Status color scheme** (this project):
  - 🟢 Green (0, 200, 0): Normal operation
  - 🔵 Blue (0, 0, 255): Edit mode active
  - 🔴 Red (255, 0, 0): Paused/error
  - ⚫ Off (0, 0, 0): Idle
- **Common issues**:
  - **No output**: Check data pin, verify 5V power, try different GPIO
  - **Wrong colors**: Adjust color order flag (NEO_GRB vs NEO_RGB)
  - **Flickering**: Add 470Ω resistor on data line, add capacitor (1000µF) on power
  - **Dim at 3.3V**: WS2812B needs ~3.5V logic high; add level shifter or use 5V-tolerant pin
- **Best practices**:
  - Call `begin()` in setup
  - Call `show()` after every color/brightness change to update LED
  - Use `setBrightness()` globally rather than scaling RGB values manually
  - For multiple LEDs, update all pixels then call `show()` once
- **Power considerations**:
  - Single LED at 50% brightness: ~30mA (safe from ESP32 GPIO)
  - Multiple LEDs or full brightness: Use external 5V supply
- **Lessons learned**:
  - Brightness 50/255 (~20%) is plenty for a status indicator
  - User control (10-100%) gives flexibility without being blinding
  - setBrightness() + show() must be called to apply changes immediately

### Output: MOSFET + 12V Solenoid Valve
- **Control Pin**: GPIO 4
- **Load**: Normally-closed solenoid (opens when powered)
- **Safety**: Requires safety input (GPIO 17) and spindle running signal before activating

### Power & Safety Input
- **Safety Input**: GPIO 17 (INPUT_PULLUP)
- **Function**: Prevents air burst if safety not OK or spindle not running

## Persistent Storage (NVS)
- **Method**: ESP32 Non-Volatile Storage (NVS) via Arduino `Preferences` library
- **Namespace**: "airblast" (change for different projects to avoid conflicts)
- **Stored settings** (this project):
  - `pulseOnMs` (UInt): Solenoid ON time (milliseconds)
  - `pulseOffMs` (UInt): Solenoid OFF time (milliseconds)
  - `oledBrightness` (UChar): OLED contrast (0-255)
  - `ledBrightness` (UChar): NeoPixel brightness (0-255, user sees 10-100%)
- **Usage pattern**:
  ```cpp
  Preferences preferences;
  
  // Read
  preferences.begin("namespace", true); // true = read-only
  uint32_t value = preferences.getUInt("key", defaultValue);
  preferences.end();
  
  // Write
  preferences.begin("namespace", false); // false = read-write
  preferences.putUInt("key", value);
  preferences.end();
  ```
- **Best practices**:
  - Always call `end()` after use to free resources
  - Use read-only mode when only reading
  - Provide sensible defaults for missing keys
  - Keep key names short (15 char max recommended)
  - Call `saveSettings()` after user adjustments, not on every encoder step
- **Factory reset**:
  - `preferences.clear()` erases all keys in namespace
  - Or restore individual defaults and call `putXxx()` for each
- **Storage limits**:
  - NVS partition: typically 20KB-64KB (check partition table)
  - Individual value size: up to 1984 bytes
  - Key name: max 15 characters
- **Common issues**:
  - **Settings not persisting**: Check `end()` is called, verify namespace spelling
  - **NVS full**: Use `preferences.clear()` or reduce stored data
  - **Corrupted partition**: Re-flash firmware with erase option
- **Lessons learned**:
  - Save on setting change, not on every encoder rotation (reduces wear)
  - Validate loaded values (constrain to valid ranges)
  - Factory reset function is essential for testing/recovery

## Pin Assignments Summary (This Project)

| Function | GPIO | Type | Notes |
|----------|------|------|-------|
| OLED SDA | 21 | I2C | Default I2C data |
| OLED SCL | 22 | I2C | Default I2C clock |
| Encoder CLK | 25 | Input | Quadrature A |
| Encoder DT | 26 | Input | Quadrature B |
| Encoder SW | 27 | Input | Push button (pulldown/pullup) |
| NeoPixel | 5 | Output | WS2812 data |
| Solenoid | 4 | Output | MOSFET gate |
| Safety Input | 17 | Input | Pullup, future use |

**GPIO still available**: 13, 14, 15, 16, 18, 19, 23, 32, 33, 34-39 (input-only)

## Button Input Patterns

### Single-Click Detection
- Press and release within `LONG_PRESS_MS` (1000ms)
- Wait `DOUBLE_CLICK_MS` (400ms) to confirm no second click
- Action: Toggle mode or enter edit

### Double-Click Detection
- Two presses within `DOUBLE_CLICK_MS` of each other
- Action: Enter/exit menu

### Long-Press Detection
- Press held for ≥ `LONG_PRESS_MS` (1000ms)
- **Trigger while holding** (not on release) for immediate response
- Action: Pause/resume or exit menu

### Debouncing
- `BUTTON_DEBOUNCE_MS` = 50ms
- Ignore any state changes within debounce window
- Track last event time and reject rapid transitions

## Known Issues & Workarounds

### Display
- **Issue**: Text size 2+ easily overflows 128px width
- **Workaround**: Calculate pixel width before layout; use size 1 for most text
- **Formula**: `text_width = strlen(text) × (6 × size)`

### Encoder
- **Issue**: Occasional "double-stepping" or skipped steps due to noise
- **Workaround**: State machine with illegal transition rejection + 4 transitions per detent
- **Future**: Add hardware RC filter (100nF cap on CLK/DT to ground)

### NeoPixel at 3.3V
- **Issue**: WS2812 may not recognize 3.3V as logic high (needs ~3.5V)
- **Workaround**: Works in practice with short wire; add level shifter for reliability
- **Alternative**: Use 3.3V-compatible LEDs (SK6812 or APA102)

## Timing Constants Reference

| Constant | Value | Purpose |
|----------|-------|---------|
| `LONG_PRESS_MS` | 1000ms | Long-press threshold |
| `DOUBLE_CLICK_MS` | 400ms | Double-click window |
| `BUTTON_DEBOUNCE_MS` | 50ms | Button debounce time |
| `ENCODER_EVENT_MIN_MS` | 5ms | Min encoder transition spacing |
| `EDIT_TIMEOUT_MS` | 5000ms | Auto-exit edit mode (main screen) |
| `TIMING_STEP_MS` | 250ms | Timing adjustment increment |
| `TIMING_MIN_MS` | 250ms | Minimum pulse timing |
| `TIMING_MAX_MS` | 1500ms | Maximum pulse timing |

## Generic Reusable Code Patterns

### Quadrature Encoder Decoder (see `encoder_handler.cpp`)
- Accumulates 4 transitions per detent
- Rejects illegal transitions
- Returns +1 (CW) or -1 (CCW) per logical step

### Multi-Action Button Handler (see `button_handler.cpp`)
- Debounced input
- Single-click, double-click, long-press detection
- Long-press triggers while holding (not on release)

### Hash-Based Display Update (see `display_controller.cpp`)
- Calculate hash of display state
- Only redraw if hash changes
- Force redraw on mode transitions

### Settings Persistence Pattern (see `settings.cpp`)
- `loadSettings()` on boot with defaults
- `saveSettings()` on user change
- `factoryReset()` for recovery
- Validate all loaded values

## Future Project Ideas Using This Hardware

- **Temperature controller** (add thermocouple via MAX31855)
- **Oven timer** with presets and temp monitoring
- **Irrigation controller** with scheduling
- **Light timer** with adjustable on/off cycles
- **Motor speed controller** with PWM output
- **Data logger** with SD card or WiFi upload
- **Multi-zone thermostat** with relay outputs

## References & Datasheets

- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [ESP32 GPIO Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)
- [SSD1306 Display Library](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Graphics Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
- [WS2812 Datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf)
- [KY-040 Encoder Info](http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/keyes-ky-040-arduino-rotary-encoder-user-manual/)
- [PlatformIO Documentation](https://docs.platformio.org/)

## Component Suppliers & Part Numbers

*(Add specific suppliers and part numbers as needed for future reference)*

- **ESP32-WROOM-32**: Various dev boards available (NodeMCU-32S, DevKitC, etc.)
- **0.96" OLED SSD1306**: Generic modules widely available
- **KY-040 Encoder**: Common rotary encoder module
- **WS2812B LED**: Individual or strip form factor
- **MOSFET module**: Generic logic-level MOSFET breakout
