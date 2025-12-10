# CNC Air Blast Controller

ESP32-based controller for automatic chip clearing on CNC machines using timed compressed-air bursts.

## Why This Exists

CNC routers generate a lot of chips and dust that can:
- Obscure the cutting path and workpiece
- Cause poor cut quality when chips re-cut
- Clog up the work area and reduce spindle cooling
- Make it difficult to see what's happening

Traditional solutions (shop vac, flood coolant) are messy, noisy, or impractical for small hobby machines. This controller uses **timed compressed air bursts** to:
- Clear chips automatically during operation
- Provide precise control over blast timing
- Adjust on-the-fly without stopping your job
- Run silently between bursts (unlike a constant vacuum)

Originally built for a Stepcraft M1000, but applicable to any CNC router, mill, or similar machine with compressed air available.

## Features

- **Adjustable pulse timing** - Configure ON/OFF durations (250-1500ms) for optimal chip clearing
- **Menu system** - Navigate settings with double-click access:
  - LED brightness control (10-100%)
  - Device info display
- **Smart input handling**:
  - Single-click to toggle edit mode or select menu items
  - Double-click to enter/exit menu
  - Long-press (≥1s) to pause/resume or exit menu
  - Encoder navigation and adjustment
- **Visual feedback**:
  - RGB LED status indicator (green=active, blue=editing, red=paused)
  - OLED display shows state and timing values
  - Highlight/inversion shows active selection
- **Persistent settings** - All configurations saved to ESP32 NVS
- **Robust input filtering** - Quadrature decoder with illegal transition rejection, button debouncing

## Hardware

- **MCU**: ESP32-WROOM-32 dev board (240MHz dual-core, 4MB flash, 320KB RAM)
- **Display**: 0.96" OLED SSD1306, 128x64 pixels, I2C
- **Input**: KY-040 rotary encoder with push button
- **LED**: WS2812 RGB status indicator
- **Output**: MOSFET module driving 12V normally-closed solenoid valve
- **Safety**: Safety input (future integration)

### Pin Connections

| Component | Pin | ESP32 GPIO | Notes |
|-----------|-----|------------|-------|
| OLED SDA | SDA | 21 | I2C data |
| OLED SCL | SCL | 22 | I2C clock |
| Encoder CLK | CLK | 25 | Quadrature A |
| Encoder DT | DT | 26 | Quadrature B |
| Encoder SW | SW | 27 | Push button |
| NeoPixel | DIN | 5 | WS2812 data |
| Solenoid | OUT | 4 | MOSFET gate |
| Safety Input | IN | 17 | Future use |

All components use 3.3V logic. See [HARDWARE_NOTES.md](HARDWARE_NOTES.md) for detailed wiring information, component specifications, and lessons learned.

## How It Works

1. **Solenoid valve** opens/closes a compressed air line on a timed schedule
2. **Controller** manages pulse timing (ON: 250-1500ms, OFF: 250-1500ms)
3. **User interface** allows real-time adjustment via rotary encoder and OLED display
4. **Settings persist** across power cycles using ESP32 non-volatile storage

The modular code structure separates concerns (display, input, output, settings) for easy maintenance and future expansion.

## Usage

### Main Screen
- **Display shows**: System state (RUN/IDLE), ON time, OFF time
- **Single-click**: Toggle between adjusting ON vs OFF time (enters edit mode)
- **Rotate encoder** (in edit mode): Adjust timing in 250ms steps (250-1500ms range)
- **Long-press** (≥1s): Pause/resume pulsing
- **Double-click**: Enter menu
- **Edit timeout**: Auto-exits edit mode after 5 seconds of inactivity

### Menu System
- **Double-click**: Enter/exit menu
- **Rotate encoder**: Navigate between menu items
- **Single-click**: Enter/exit edit mode for selected item
- **Long-press** (≥1s): Exit menu
- **Menu items**:
  - **LED**: Adjust status LED brightness (10-100% in 10% steps)
  - **About**: View firmware version and device info

### LED Status Indicators
- 🟢 **Green**: Running normally (active, safety OK, spindle running)
- 🔵 **Blue**: Adjusting parameters (edit mode active)
- 🔴 **Red**: System paused
- ⚫ **Off**: Idle or not active

## Technical Details

- **Encoder handling**: Quadrature decoding with 4 transitions per detent, illegal transition rejection
- **Button debouncing**: Minimum event spacing of 50ms
- **Double-click detection**: 400ms window
- **Long-press threshold**: 1000ms
- **Edit timeout**: 5000ms (main screen only, menu has manual control)
- **Display refresh**: Hash-based change detection to minimize flicker
- **NVS persistence**: ON/OFF timing, LED brightness, OLED brightness
- **Upload speed**: 921600 baud
- **Serial monitor**: 115200 baud

## Future Enhancements

- [ ] Spindle input integration (hardware ready on GPIO, software stubbed)
- [ ] Timing presets for different materials (wood, metal, plastic)
- [ ] Manual pulse trigger for testing
- [ ] Pulse counter display and statistics
- [ ] Error handling with visual feedback

## License

MIT License - See LICENSE file for details

## Author

ecpunk  
GitHub: [github.com/ecpunk/CNC-Air-Blaster](https://github.com/ecpunk/CNC-Air-Blaster)
