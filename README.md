# CNC Air Blast Controller

ESP32-based controller for automatic chip clearing on a Stepcraft M1000 CNC machine using timed compressed-air bursts.

## Hardware

- **MCU**: ESP32-WROOM-32 dev board
- **Encoder**: KY-040 rotary encoder with push button (GPIO 25/26/27)
- **Display**: 0.96" OLED SSD1306 (I2C: GPIO 21/22)
- **LED**: Single WS2812 RGB status indicator (GPIO 5)
- **Output**: MOSFET module driving 12V normally-closed solenoid valve (GPIO 4)

## Building and Uploading

### Prerequisites

Install [PlatformIO](https://platformio.org/install):
- **VS Code Extension** (recommended): Search for "PlatformIO IDE" in VS Code extensions
- **Or CLI**: `pip install platformio`

### Quick Start

1. **Clone the repository** (first time only):
   ```bash
   git clone https://github.com/ecpunk/CNC-Air-Blaster.git
   cd CNC-Air-Blaster
   ```

2. **Update to latest** (subsequent times):
   ```bash
   git pull
   ```

3. **Build and upload**:
   ```bash
   # Via CLI
   pio run -t upload

   # Or via VS Code: Ctrl+Alt+U (Upload) or Ctrl+Shift+B (Build)
   ```

4. **Monitor serial output**:
   ```bash
   pio device monitor
   ```

### Configure Serial Port

Edit `platformio.ini` and set `upload_port` to your board's port:
- **Windows**: `COM3`, `COM4`, etc. (check Device Manager)
- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`, etc.
- **macOS**: `/dev/cu.usbserial-*` or `/dev/cu.SLAB_USBtoUART`

Or let PlatformIO auto-detect by commenting out the `upload_port` line.

## Project Structure

```
CNC-Air-Blaster/
├── CNC_Air_Blaster.ino         # Main sketch (setup/loop)
├── hardware.h                   # Pin definitions & constants
├── state.h/cpp                  # Global state & enums
├── settings.h/cpp               # Preferences (NVS) persistence
├── encoder_handler.h/cpp        # Rotary encoder logic
├── button_handler.h/cpp         # Button debounce & long-press
├── led_controller.h/cpp         # WS2812 RGB LED control
├── display_controller.h/cpp     # OLED display management
├── pulse_controller.h/cpp       # Solenoid pulsing state machine
├── platformio.ini               # PlatformIO configuration
└── README.md                    # This file
```

## Usage

**Short press** encoder button
- Toggles between adjusting ON time and OFF time

**Long press** encoder button (≥ 1 second)
- Pause / Resume pulsing

**Rotate encoder**
- Adjust selected timing value in 250 ms steps
- CW = increase, CCW = decrease
- Range: 250–1500 ms

**LED Status**
- 🟢 Green: Running normally
- 🔵 Blue: Adjusting ON time
- 🟣 Purple: Adjusting OFF time
- 🔴 Red: Paused
- ⚫ Off: Idle or not active

**Display**
- Shows current state (RUN, PAUSED, or IDLE)
- Shows ON and OFF timing values in milliseconds
- Underline indicates which value is being edited

## Features

- Real-time timing adjustment without stopping CNC
- Persistent settings (saved to ESP32 NVS)
- Robust encoder decoding (1 step per detent)
- Debounced button input with long-press detection
- Clear visual status feedback via RGB LED
- OLED display for timing and state
- Modular code structure for easy maintenance

## Future Enhancements

- Spindle input gating (only pulse when spindle is running)
- Safety interlock input
- Configurable step size for timing adjustments
