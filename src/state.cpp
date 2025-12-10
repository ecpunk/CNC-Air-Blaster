#include "state.h"

// ============================================================================
// GLOBAL STATE VARIABLE DEFINITIONS
// ============================================================================

// Timing values (in milliseconds)
unsigned long pulseOnMs  = 500;
unsigned long pulseOffMs = 500;

// State machine
AdjustMode  adjustMode  = ADJUST_ON_TIME;
SystemState systemState = SYS_ACTIVE;
MenuState   menuState   = MENU_NONE;

// Edit mode tracking
bool inEditMode          = false;
unsigned long lastEditTime = 0;

// Menu control
uint8_t menuSelection = 0;
uint8_t maxMenuItems = 1;  // Currently: 0 = brightness

// Button state
bool buttonPressed         = false;
unsigned long buttonPressTime = 0;
unsigned long lastButtonEvent  = 0;
unsigned long lastButtonReleaseTime = 0;
uint8_t buttonClickCount = 0;
bool pauseActive = false;

// Encoder state
uint8_t  encLastState   = 0;
int8_t   encMovement    = 0;
unsigned long lastEncEvent = 0;

// Pulse tracking
bool pulsing            = false;
unsigned long lastPulseTime = 0;
unsigned long pulseCount    = 0;

// Safety/spindle
bool safetyOK          = true;
bool spindleRunning    = true;

// Display and LED
uint8_t oledBrightness = 0x7F;
bool displayOK = false;
