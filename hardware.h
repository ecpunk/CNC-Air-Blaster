#ifndef HARDWARE_H
#define HARDWARE_H

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
const int ENCODER_CLK = 25;
const int ENCODER_DT  = 26;
const int ENCODER_SW  = 27;
const int LED_PIN     = 5;   // WS2812 data
const int MOSFET_PIN  = 4;
const int SAFE_INPUT_PIN = 14;
const int SPINDLE_PIN = 13;  // reserved for future use

// ============================================================================
// OLED CONFIG
// ============================================================================
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;
const int SCREEN_ADDRESS = 0x3C;

// ============================================================================
// TIMING CONSTANTS
// ============================================================================
const unsigned long TIMING_MIN_MS   = 250;
const unsigned long TIMING_MAX_MS   = 1500;
const unsigned long TIMING_STEP_MS  = 250;

const unsigned long BUTTON_DEBOUNCE_MS = 60;
const unsigned long LONG_PRESS_MS      = 1000;   // pause / resume threshold
const unsigned long EDIT_TIMEOUT_MS    = 5000;   // underline + LED edit color timeout
const unsigned long ENCODER_EVENT_MIN_MS = 1;   // ignore ultra-fast jitter

const uint8_t BRIGHTNESS_DEFAULT = 0x7F;

#endif
