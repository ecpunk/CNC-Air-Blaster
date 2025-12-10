#include <Arduino.h>
#include "hardware.h"
#include "state.h"
#include "settings.h"
#include "display_controller.h"
#include "led_controller.h"

void initEncoder() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);

  // Initialize encoder state from current pins
  uint8_t clk = digitalRead(ENCODER_CLK);
  uint8_t dt  = digitalRead(ENCODER_DT);
  encLastState = (clk << 1) | dt;
  encMovement  = 0;
}

// --- Quadrature decoder: 1 logical step per full detent ---
// Handles direction and updates timing values or menu items
static void handleEncoderStep(int direction) {
  // direction: +1 = CW, -1 = CCW
  unsigned long now = millis();

  // If in menu, navigate menu items
  if (menuState == MENU_ACTIVE) {
    // If we're editing a menu item, apply the encoder to that item
    if (inEditMode) {
      // Adjust the selected menu item
      if (menuSelection == MENU_BRIGHTNESS) {
        // LED brightness: 10-100% in 10% steps, mapped to 26-255
        // Current brightness as percentage (rounded to nearest 10%)
        int percent = (int)((ledBrightness * 100.0 / 255.0) + 0.5);
        percent = ((percent + 5) / 10) * 10; // Round to nearest 10%
        
        // Apply encoder delta
        int delta = (direction > 0) ? 10 : -10;
        percent += delta;
        if (percent < 10) percent = 10;
        if (percent > 100) percent = 100;
        
        // Map percentage to NeoPixel brightness (0-255)
        uint8_t newB = (uint8_t)(percent * 255 / 100);
        setStatusLedBrightness(newB);
        saveSettings();
        updateDisplay();
      }
      // Info/About is not editable
    } else {
      // Navigate between menu items (only 2 now)
      if (direction > 0) {
        menuSelection = (menuSelection + 1) % 2;
      } else {
        menuSelection = (menuSelection == 0) ? 1 : (menuSelection - 1);
      }
      Serial.print("Menu Selection: ");
      Serial.println(menuSelection);
      updateDisplay();
    }
    return;
  }

  // Only allow encoder to adjust values if inEditMode is true
  if (!inEditMode) {
    // Ignore encoder turns unless in edit mode (single click enters edit mode)
    return;
  }
  lastEditTime = now;

  if (adjustMode == ADJUST_ON_TIME) {
    long next = (long)pulseOnMs + (direction > 0 ? TIMING_STEP_MS : -TIMING_STEP_MS);
    pulseOnMs = constrain(next, TIMING_MIN_MS, TIMING_MAX_MS);
  } else {
    long next = (long)pulseOffMs + (direction > 0 ? TIMING_STEP_MS : -TIMING_STEP_MS);
    pulseOffMs = constrain(next, TIMING_MIN_MS, TIMING_MAX_MS);
  }

  saveSettings();
  updateDisplay();

  Serial.print("ENC STEP ");
  Serial.print(direction > 0 ? "CW" : "CCW");
  Serial.print("  ON=");
  Serial.print(pulseOnMs);
  Serial.print("  OFF=");
  Serial.println(pulseOffMs);
}

// --- Improved encoder filter: reject illegal transitions & jitter ---
void handleEncoder() {
  unsigned long now = millis();
  if (now - lastEncEvent < ENCODER_EVENT_MIN_MS) return;

  uint8_t clk = digitalRead(ENCODER_CLK);
  uint8_t dt  = digitalRead(ENCODER_DT);
  uint8_t state = (clk << 1) | dt;

  if (state == encLastState) return;

  lastEncEvent = now;

  // Quadrature decoding table:
  // Maps transitions from previous state to next state
  // Returns delta: +1 (CW step), -1 (CCW step), 0 (invalid)
  static const int8_t table[4][4] = {
    {  0, +1, -1,  0 },
    { -1,  0,  0, +1 },
    { +1,  0,  0, -1 },
    {  0, -1, +1,  0 }
  };

  int8_t delta = table[encLastState][state];

  // Ignore impossible transitions entirely
  if (delta == 0 && state != encLastState) {
    encLastState = state;
    return;
  }

  encLastState = state;
  encMovement += -delta;  // invert encoder direction (CW increases delay)

  // Accumulate 4 transitions per detent before triggering step
  if (encMovement >= 4) {
    encMovement = 0;
    handleEncoderStep(+1);
  } else if (encMovement <= -4) {
    encMovement = 0;
    handleEncoderStep(-1);
  }
}
