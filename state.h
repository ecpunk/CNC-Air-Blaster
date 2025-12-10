#ifndef STATE_H
#define STATE_H

// ============================================================================
// ENUMS
// ============================================================================
enum AdjustMode  { ADJUST_ON_TIME, ADJUST_OFF_TIME };
enum SystemState { SYS_IDLE, SYS_ACTIVE, SYS_PAUSED };

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// Timing values (in milliseconds)
extern unsigned long pulseOnMs;
extern unsigned long pulseOffMs;

// State machine
extern AdjustMode  adjustMode;
extern SystemState systemState;

// Edit mode tracking
extern bool inEditMode;
extern unsigned long lastEditTime;

// Button state
extern bool buttonPressed;
extern unsigned long buttonPressTime;
extern unsigned long lastButtonEvent;

// Encoder state
extern uint8_t  encLastState;
extern int8_t   encMovement;
extern unsigned long lastEncEvent;

// Pulse tracking
extern bool pulsing;
extern unsigned long lastPulseTime;
extern unsigned long pulseCount;

// Safety/spindle
extern bool safetyOK;
extern bool spindleRunning;

// Display and LED
extern uint8_t oledBrightness;
extern bool displayOK;

#endif
