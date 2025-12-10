#include <Arduino.h>
#include "hardware.h"
#include "state.h"

void initPulsing() {
  pinMode(MOSFET_PIN, OUTPUT);
  digitalWrite(MOSFET_PIN, LOW);
}

// --- Pulsing state machine ---
//
// Alternates between two states:
// 1. OFF (pulsing=false): waits for pulseOffMs, then goes HIGH
// 2. ON  (pulsing=true):  waits for pulseOnMs,  then goes LOW
//
void doPulsing() {
  unsigned long now = millis();

  if (!pulsing) {
    // Currently OFF: check if it's time to turn ON
    if (now - lastPulseTime >= pulseOffMs) {
      pulsing = true;
      lastPulseTime = now;
      digitalWrite(MOSFET_PIN, HIGH);
    }
  } else {
    // Currently ON: check if it's time to turn OFF
    if (now - lastPulseTime >= pulseOnMs) {
      pulsing = false;
      lastPulseTime = now;
      digitalWrite(MOSFET_PIN, LOW);
      pulseCount++;

      if (pulseCount % 50 == 0) {
        Serial.print("PULSES: ");
        Serial.println(pulseCount);
      }
    }
  }
}
