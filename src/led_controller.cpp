#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "hardware.h"
#include "state.h"

Adafruit_NeoPixel statusLed(1, LED_PIN, NEO_GRB + NEO_KHZ800);

void initStatusLed() {
  statusLed.begin();
  // Initialize NeoPixel brightness from persisted state variable
  statusLed.setBrightness(ledBrightness);
  statusLed.show();
}

// Set status LED brightness (0-255) and apply immediately
void setStatusLedBrightness(uint8_t b) {
  ledBrightness = b;
  statusLed.setBrightness(ledBrightness);
  statusLed.show();
}

void setLedColor(uint8_t r, uint8_t g, uint8_t b) {
  statusLed.setPixelColor(0, statusLed.Color(r, g, b));
  statusLed.show();
}

uint32_t wheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return statusLed.Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return statusLed.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return statusLed.Color(pos * 3, 255 - pos * 3, 0);
}

void rainbowBoot() {
  for (int i = 0; i < 64; i++) {
    statusLed.setPixelColor(0, wheel(i * 4));
    statusLed.show();
    delay(15);
  }
}

void updateStatusLed() {
  unsigned long now = millis();
  bool editWindow = inEditMode && (now - lastEditTime < EDIT_TIMEOUT_MS);

  if (systemState == SYS_PAUSED) {
    // Red = paused
    setLedColor(255, 0, 0);
  } else if (editWindow) {
    // Blue = adjusting (ON or OFF time)
    setLedColor(0, 0, 255);
  } else if (systemState == SYS_ACTIVE && safetyOK && spindleRunning) {
    // Green = normal running
    setLedColor(0, 200, 0);
  } else {
    // Off = idle / not active
    setLedColor(0, 0, 0);
  }
}
