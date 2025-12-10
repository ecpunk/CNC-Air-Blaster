// ============================================================================
// CNC Air Blast Controller - Modular Version
// ============================================================================
// - Rotary encoder: adjusts ON/OFF timing in 250 ms steps
// - Button short press: toggle ON/OFF adjustment
// - Button long press (>=1s): Pause / Resume
// - WS2812 LED: status indicator
// - 0.96" OLED: display timing and state
// - Pulsed solenoid control: drives compressed air
// ============================================================================

#include "hardware.h"
#include "state.h"
#include "settings.h"
#include "led_controller.h"
#include "encoder_handler.h"
#include "button_handler.h"
#include "display_controller.h"
#include "pulse_controller.h"

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(200);

  // Initialize all subsystems
  initEncoder();
  initButton();
  initStatusLed();
  initDisplay();
  initPulsing();

  // Set up safety/spindle input
  pinMode(SAFE_INPUT_PIN, INPUT_PULLUP);

  // Load persistent settings from NVS
  loadSettings();

  // Configure display
  if (displayOK) {
    setDisplayBrightness();
    updateDisplay();
  }

  // Perform boot animation
  rainbowBoot();

  // Start in active state
  systemState = SYS_ACTIVE;
  updateStatusLed();

  Serial.println("=================================");
  Serial.println("CNC Air Blast Controller - MODULAR");
  Serial.println("Short press = swap ON/OFF adjust");
  Serial.println("Long  press = pause / resume");
  Serial.println("=================================");
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  unsigned long now = millis();

  // --- Poll inputs ---
  safetyOK = digitalRead(SAFE_INPUT_PIN) == HIGH;
  spindleRunning = true;  // TODO: add spindle input gating logic

  handleEncoder();
  handleButton();

  // --- Update display state ---
  // Auto-exit edit mode timeout only applies when NOT in menu
  if (inEditMode && menuState == MENU_NONE && (now - lastEditTime > EDIT_TIMEOUT_MS)) {
    inEditMode = false;
    updateDisplay();
  }

  // --- Control solenoid output ---
  if (systemState == SYS_ACTIVE && safetyOK && spindleRunning) {
    doPulsing();
  } else {
    // Safety override: always turn off output if not active/safe
    digitalWrite(MOSFET_PIN, LOW);
  }

  // --- Update status LED ---
  updateStatusLed();
}
