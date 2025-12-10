#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "hardware.h"
#include "state.h"
#include "led_controller.h"

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

void drawPauseScreen() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 20);
  display.println("PAUSE");
  display.display();
}

void drawBrightnessMenu() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("LED Brightness");
  display.setCursor(0, 16);
  display.print("Level: ");
  display.println(oledBrightness);
  display.setCursor(0, 32);
  display.println("Rotate to adjust");
  display.setCursor(0, 48);
  display.println("(0-255)");
  
  display.display();
}

void drawMenuList() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Title
  display.setCursor(0, 0);
  display.println("MENU");

  // Menu items with size 2 text
  const char* items[] = {"LED", "About"};
  for (int i = 0; i < 2; i++) {
    int y = 16 + (i * 24);
    display.setTextSize(2);
    if (i == menuSelection) {
      // Highlight selected item with inversion
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
      display.setCursor(0, y);
      display.print("> ");
      display.print(items[i]);
      if (i == MENU_BRIGHTNESS) {
        // Show brightness as percent (0-255 mapped to 0-100%)
        int percent = (int)((ledBrightness * 100.0 / 255.0) + 0.5);
        display.print(":");
        display.print(percent);
        display.print("%");
      }
      display.setTextColor(SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, y);
      display.print("  ");
      display.print(items[i]);
    }
  }
  display.display();
}

void drawInfoAbout() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("CNC Air Blaster");
  display.println("Firmware v1.0");
  display.println("by ecpunk");
  display.println("");
  display.println("github.com/ecpunk");
  display.display();
}

void drawMenuItem2() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("Menu Item 2");
  display.setCursor(0, 16);
  display.println("[Placeholder]");
  display.setCursor(0, 32);
  display.println("Function TBD");
  
  display.display();
}

void drawMenuItem3() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("Menu Item 3");
  display.setCursor(0, 16);
  display.println("[Placeholder]");
  display.setCursor(0, 32);
  display.println("Function TBD");
  
  display.display();
}

void drawMenuItem4() {
  if (!displayOK) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0, 0);
  display.println("Menu Item 4");
  display.setCursor(0, 16);
  display.println("[Placeholder]");
  display.setCursor(0, 32);
  display.println("Function TBD");
  display.setCursor(0, 48);
  display.println("Up/Dn: Navigate");
  
  display.display();
}

void drawMenu() {
  // If Info/About selected and in edit mode, show info screen
  if (menuSelection == MENU_INFO && inEditMode) {
    drawInfoAbout();
  } else {
    drawMenuList();
  }
}

void drawMainScreen() {
  if (!displayOK) return;
  display.clearDisplay();

  // STATE line (size 1 to fit)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (systemState == SYS_ACTIVE) display.print("RUN");
  else                           display.print("IDLE");

  // ON value (left side) - highlight if adjusting AND in edit mode
  display.setTextSize(2);
  display.setCursor(0, 16);
  if (adjustMode == ADJUST_ON_TIME && inEditMode) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Inverted
    display.print("ON:");
    display.print(pulseOnMs);
    display.setTextColor(SSD1306_WHITE); // Reset to normal
  } else {
    display.print("ON:");
    display.print(pulseOnMs);
  }

  // OFF value (below ON) - highlight if adjusting AND in edit mode
  display.setCursor(0, 40);
  if (adjustMode == ADJUST_OFF_TIME && inEditMode) {
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
  // If paused, show pause screen
  if (systemState == SYS_PAUSED) {
    drawPauseScreen();
    lastDisplayHash = 0;  // Reset hash to force redraw when unpaused
    return;
  }
  
  // If in menu, show menu
  if (menuState == MENU_ACTIVE) {
    drawMenu();
    lastDisplayHash = 0;  // Reset hash to force redraw when exiting menu
    return;
  }
  
  // Otherwise show main screen
  unsigned long h = pulseOnMs ^ (pulseOffMs << 8) ^ (adjustMode << 16) ^ (systemState << 20) ^ (inEditMode << 24);
  if (h == lastDisplayHash) return;
  lastDisplayHash = h;
  drawMainScreen();
}
