# Hardware Notes & Lessons Learned

## Overview
This document captures hardware-specific insights, quirks, and lessons learned during development of the CNC Air Blaster controller. Use this for future projects using the same components.

## Components

### MCU: ESP32-WROOM-32
- **Operating Voltage**: 3.3V
- **Flash**: 4MB
- **RAM**: 320KB
- **Clock**: 240MHz dual-core
- **Notes**: 
  - Upload speed set to 921600 baud for faster development
  - Monitor speed: 115200 baud

### Display: 0.96" OLED SSD1306 (I2C)
- **Resolution**: 128x64 pixels
- **Interface**: I2C (GPIO 21: SDA, GPIO 22: SCL)
- **Address**: 0x3C
- **Considerations**:
  - Text size 2 is too large for the small display - use size 1 or smaller
  - Default brightness adequate but can be adjusted via contrast command
  - Library: Adafruit SSD1306 v2.5.9+

### Input: KY-040 Rotary Encoder with Push Button
- **Pins**: 
  - CLK: GPIO 25
  - DT: GPIO 26
  - SW: GPIO 27
- **Debounce Issues & Solutions**:
  - *Issue*: [Document specific debounce problems encountered]
  - *Solution*: [Document the fix or workaround]
  - *Timing*: [Record any critical timing values]

### LED: WS2812 RGB (NeoPixel)
- **Pin**: GPIO 5
- **Protocol**: SPI (NEO_GRB + NEO_KHZ800)
- **Brightness**: Currently set to 50 (0-255 scale)
- **Status Colors**:
  - Red: System paused
  - Blue: Adjusting parameters (ON or OFF time)
  - Green: Normal running (active, safety OK, spindle running)
  - Off: Idle

### Output: MOSFET + 12V Solenoid Valve
- **Control Pin**: GPIO 4
- **Load**: Normally-closed solenoid (opens when powered)
- **Safety**: Requires safety input (GPIO 17) and spindle running signal before activating

### Power & Safety Input
- **Safety Input**: GPIO 17 (INPUT_PULLUP)
- **Function**: Prevents air burst if safety not OK or spindle not running

## Persistent Storage
- **Method**: NVS (Non-Volatile Storage via Preferences library)
- **Stored**: ON/OFF timing values, brightness settings
- **Namespace**: Default Arduino Preferences

## Known Issues & Workarounds

### Display
- Text size 2 overflows on 0.96" screen
- **Workaround**: Use text size 1 or implement custom layout

## Future Improvements & Considerations

- [ ] Investigate optimal debounce timing for encoder
- [ ] Document any timing-critical sections
- [ ] Record component supplier info and part numbers
- [ ] Add pin conflict checking for future revisions

## References

- [KY-040 Encoder Datasheet](#)
- [SSD1306 Display Library](https://github.com/adafruit/Adafruit_SSD1306)
- [ESP32 GPIO Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html)
