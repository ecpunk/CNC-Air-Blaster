#ifndef PULSE_CONTROLLER_H
#define PULSE_CONTROLLER_H

// Initialize MOSFET output pin
void initPulsing();

// Main pulsing logic - call from loop()
// Generates timed HIGH/LOW pulses on MOSFET_PIN
void doPulsing();

#endif
