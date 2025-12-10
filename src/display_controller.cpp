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

  // STATE line
  display.setTextSize(2);
  display.setCursor(0, 0);
  if (systemState == SYS_PAUSED)      display.print("STATE: PAUSED");
  else if (systemState == SYS_ACTIVE) display.print("STATE: RUN");
  else                                display.print("STATE: IDLE");

  // ON label
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print("ON");

  // OFF label
  display.setCursor(74, 20);
  display.print("OFF");

  // ON time
  display.setCursor(0, 38);
  display.print(pulseOnMs);
  display.print(" ms");

  // OFF time
  display.setCursor(74, 38);
  display.print(pulseOffMs);
  display.print(" ms");

  // Underline bars
  if (adjustMode == ADJUST_ON_TIME) {
    display.fillRect(0, 56, 60, 4, SSD1306_WHITE);
  }
  if (adjustMode == ADJUST_OFF_TIME) {
    display.fillRect(74, 56, 60, 4, SSD1306_WHITE);
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
