#include "hardware.h"
#include "state.h"
#include "display_controller.h"

void initButton() {
  // --- Auto-detect pull-up vs floating SW pin ---
  pinMode(ENCODER_SW, INPUT_PULLUP);
  delay(5);
  if (digitalRead(ENCODER_SW) == HIGH) {
    // Likely floating; keep pull-up
    Serial.println("SW uses internal PULLUP");
  } else {
    // External pull-down detected
    pinMode(ENCODER_SW, INPUT);
    Serial.println("SW uses external pulldown");
  }
}

// --- Revised button state machine (clean) ---
//
// This implements a simple debounce + long-press detector:
// 1. Rising edge (LOW->HIGH): record press time
// 2. Falling edge (HIGH->LOW): measure duration
//    - If duration >= LONG_PRESS_MS: toggle pause/resume (systemState)
//    - Otherwise: toggle adjustment mode (ON <-> OFF)
//
void handleButton() {
  static bool lastRaw = false;
  unsigned long now = millis();
  bool raw = digitalRead(ENCODER_SW) == HIGH;

  // Reject changes faster than debounce window
  if (now - lastButtonEvent < BUTTON_DEBOUNCE_MS) return;

  // Rising edge: record press start time
  if (raw && !lastRaw) {
    buttonPressTime = now;
    lastButtonEvent = now;
    lastRaw = raw;
    return;
  }

  // Falling edge: measure press duration and act
  if (!raw && lastRaw) {
    lastButtonEvent = now;
    unsigned long dur = now - buttonPressTime;
    lastRaw = raw;

    if (dur >= LONG_PRESS_MS) {
      // Long press: toggle pause / resume
      systemState = (systemState == SYS_PAUSED ? SYS_ACTIVE : SYS_PAUSED);
      updateDisplay();
    } else {
      // Short press: toggle ON / OFF adjustment mode
      adjustMode = (adjustMode == ADJUST_ON_TIME ? ADJUST_OFF_TIME : ADJUST_ON_TIME);
      inEditMode = true;
      lastEditTime = now;
      updateDisplay();
    }
    return;
  }

  lastRaw = raw;
}
