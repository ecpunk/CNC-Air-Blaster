#ifndef STATE_H
#define STATE_H

#include <cstdint>

// ============================================================================
// ENUMS
// ============================================================================
enum AdjustMode  { ADJUST_ON_TIME, ADJUST_OFF_TIME };
enum SystemState { SYS_IDLE, SYS_ACTIVE, SYS_PAUSED };
enum MenuState   { MENU_NONE, MENU_BRIGHTNESS };

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// Timing values (in milliseconds)
extern unsigned long pulseOnMs;
extern unsigned long pulseOffMs;

// State machine
extern AdjustMode  adjustMode;
extern SystemState systemState;
extern MenuState   menuState;

// Edit mode tracking
extern bool inEditMode;
extern unsigned long lastEditTime;

// Menu control
extern uint8_t menuSelection;      // 0 = brightness (add more items as needed)
extern uint8_t maxMenuItems;       // total menu items

// Button state
extern bool buttonPressed;
extern unsigned long buttonPressTime;
extern unsigned long lastButtonEvent;
extern unsigned long lastButtonReleaseTime;
extern uint8_t buttonClickCount;
extern bool pauseActive;

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
