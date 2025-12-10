#ifndef DISPLAY_CONTROLLER_H
#define DISPLAY_CONTROLLER_H

// Initialize OLED display
void initDisplay();

// Set OLED brightness/contrast
void setDisplayBrightness();

// Redraw the main display screen
void drawMainScreen();

// Smart update: only redraws if state has changed
void updateDisplay();

#endif
