#include "hardware.h"
#include "state.h"
#include "settings.h"
#include "display_controller.h"

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
// Handles direction and updates timing values
static void handleEncoderStep(int direction) {
  // direction: +1 = CW, -1 = CCW
  unsigned long now = millis();

  // Allow timing edits even when paused
  inEditMode   = true;
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
