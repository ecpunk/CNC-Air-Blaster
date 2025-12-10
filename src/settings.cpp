#include <Arduino.h>
#include <Preferences.h>
#include "hardware.h"
#include "state.h"

Preferences preferences;

void loadSettings() {
  preferences.begin("airblast", true);
  pulseOnMs      = preferences.getUInt("on",  500);
  pulseOffMs     = preferences.getUInt("off", 500);
  oledBrightness = preferences.getUChar("brite", BRIGHTNESS_DEFAULT);
  preferences.end();

  pulseOnMs  = constrain(pulseOnMs,  TIMING_MIN_MS, TIMING_MAX_MS);
  pulseOffMs = constrain(pulseOffMs, TIMING_MIN_MS, TIMING_MAX_MS);

  if (oledBrightness < 16 || oledBrightness > 255) {
    oledBrightness = BRIGHTNESS_DEFAULT;
  }
}

void saveSettings() {
  preferences.begin("airblast", false);
  preferences.putUInt("on",  pulseOnMs);
  preferences.putUInt("off", pulseOffMs);
  preferences.putUChar("brite", oledBrightness);
  preferences.end();
}

void factoryReset() {
  pulseOnMs      = 500;
  pulseOffMs     = 500;
  oledBrightness = BRIGHTNESS_DEFAULT;
  saveSettings();
}
