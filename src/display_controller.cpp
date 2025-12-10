#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hardware.h"
#include "state.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay() {
  Wire.begin(21, 22);
  if (display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    displayOK = true;
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("AIR BLAST CTRL");
    display.println("Init...");
    display.display();
    delay(400);
  } else {
    displayOK = false;
    Serial.println("OLED init failed");
  }
}

void setDisplayBrightness() {
  if (!displayOK) return;
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(oledBrightness);
}

void drawMainScreen() {
  if (!displayOK) return;
  display.clearDisplay();

  // STATE line (size 1 to fit)
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (systemState == SYS_PAUSED)      display.print("PAUSED");
  else if (systemState == SYS_ACTIVE) display.print("RUN");
  else                                display.print("IDLE");

  // ON value (left side) - highlight if adjusting - size 2
  display.setTextSize(2);
  display.setCursor(0, 16);
  if (adjustMode == ADJUST_ON_TIME) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Inverted
    display.print("ON:");
    display.print(pulseOnMs);
    display.setTextColor(SSD1306_WHITE); // Reset to normal
  } else {
    display.print("ON:");
    display.print(pulseOnMs);
  }

  // OFF value (below ON) - highlight if adjusting - size 2
  display.setCursor(0, 40);
  if (adjustMode == ADJUST_OFF_TIME) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Inverted
    display.print("OFF:");
    display.print(pulseOffMs);
    display.setTextColor(SSD1306_WHITE); // Reset to normal
  } else {
    display.print("OFF:");
    display.print(pulseOffMs);
  }

  display.display();
}

unsigned long lastDisplayHash = 0;
void updateDisplay() {
  unsigned long h = pulseOnMs ^ (pulseOffMs << 8) ^ (adjustMode << 16) ^ (systemState << 20) ^ (inEditMode << 24);
  if (h == lastDisplayHash) return;
  lastDisplayHash = h;
  drawMainScreen();
}
