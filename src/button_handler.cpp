#include <Arduino.h>
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

// --- Button state machine with multiple actions ---
//
// Actions:
// 1. Single click: toggle adjust mode (ON <-> OFF)
// 2. Double click: open menu
// 3. Hold >= 1s: pause/resume or exit menu (triggers while holding, not on release)
//
void handleButton() {
  static bool lastRaw = false;
  static bool pauseTriggered = false;  // track if pause already triggered during this hold
  static unsigned long singleClickCheckTime = 0;
  unsigned long now = millis();
  bool raw = digitalRead(ENCODER_SW) == HIGH;

  // Reject changes faster than debounce window
  if (now - lastButtonEvent < BUTTON_DEBOUNCE_MS) return;

  // Rising edge (button pressed)
  if (raw && !lastRaw) {
    buttonPressTime = now;
    lastButtonEvent = now;
    pauseTriggered = false;
    lastRaw = raw;
    return;
  }

  // Button held: check for pause trigger (>= 1 second)
  if (raw && lastRaw && !pauseTriggered && (now - buttonPressTime >= LONG_PRESS_MS)) {
    // Long press detected while holding
    // If in menu, exit menu
    if (menuState == MENU_ACTIVE) {
      menuState = MENU_NONE;
      menuSelection = 0;  // Reset to first item
      inEditMode = false;
      Serial.println("Exiting menu");
      updateDisplay();
    } else {
      // Otherwise, toggle pause
      systemState = (systemState == SYS_PAUSED ? SYS_ACTIVE : SYS_PAUSED);
      Serial.print("Toggle pause. New state: ");
      Serial.println(systemState == SYS_PAUSED ? "PAUSED" : "ACTIVE");
      updateDisplay();
    }
    pauseTriggered = true;
    return;
  }

  // Falling edge (button released)
  if (!raw && lastRaw) {
    lastButtonEvent = now;
    unsigned long dur = now - buttonPressTime;
    lastRaw = raw;

    // Skip if pause was triggered (already handled)
    if (pauseTriggered) {
      pauseTriggered = false;
      return;
    }

    // Short press: detect single vs double click
    if (dur < LONG_PRESS_MS) {
      unsigned long timeSinceLastRelease = now - lastButtonReleaseTime;
      
      if (timeSinceLastRelease < DOUBLE_CLICK_MS) {
        // Potential double click
        buttonClickCount++;
        if (buttonClickCount == 2) {
          // Double click confirmed - toggle menu
          if (menuState == MENU_ACTIVE) {
            // Exit menu
            menuState = MENU_NONE;
            menuSelection = 0;
            inEditMode = false;
            Serial.println("Exiting menu via double-click");
          } else {
            // Enter menu
            menuState = MENU_ACTIVE;
            menuSelection = 0;
            Serial.println("Entering menu via double-click");
          }
          updateDisplay();
          buttonClickCount = 0;
          singleClickCheckTime = 0;
        }
      } else {
        // Single click (no pending double click)
        buttonClickCount = 1;
        singleClickCheckTime = now;
      }
      lastButtonReleaseTime = now;
    }
    return;
  }

  lastRaw = raw;
  
  // Timeout for single click confirmation
  if (buttonClickCount == 1 && singleClickCheckTime > 0) {
    if (now - lastButtonReleaseTime >= DOUBLE_CLICK_MS) {
      // Single click confirmed (no second click arrived)
      if (menuState == MENU_NONE) {
        // Not in menu, toggle adjust mode
        adjustMode = (adjustMode == ADJUST_ON_TIME ? ADJUST_OFF_TIME : ADJUST_ON_TIME);
        inEditMode = true;
        lastEditTime = now;
      } else if (menuState == MENU_ACTIVE) {
        // In menu: toggle edit mode for the selected item
        inEditMode = !inEditMode;
        lastEditTime = now;
      }
      updateDisplay();
      buttonClickCount = 0;
      singleClickCheckTime = 0;
      return;
    }
  }
}
