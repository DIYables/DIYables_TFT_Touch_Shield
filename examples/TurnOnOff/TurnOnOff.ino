/*
   Created by DIYables

   This example code is in the public domain

   Product page:
   - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
   - https://www.amazon.com/dp/B0DQ3NQ3LW

   NOTE ABOUT turnOff() / turnOn():
   - turnOff() sends Display OFF + Sleep In commands to the LCD controller.
     The screen goes white/blank, NOT dark, because the backlight LED is
     hardwired to the power rail on the shield and cannot be turned off by software.
   - turnOn() sends Sleep Out + Display ON commands. The previous content is
     restored automatically (no need to redraw).
   - To make the screen appear dark, you can call fillScreen(BLACK) before turnOff(),
     but you will need to redraw everything after turnOn().
*/

#include <DIYables_TFT_Touch_Shield.h>

#define BLACK   DIYables_TFT::colorRGB(0, 0, 0)
#define WHITE   DIYables_TFT::colorRGB(255, 255, 255)
#define BLUE    DIYables_TFT::colorRGB(0, 0, 255)
#define RED     DIYables_TFT::colorRGB(255, 0, 0)
#define GREEN   DIYables_TFT::colorRGB(0, 255, 0)
#define YELLOW  DIYables_TFT::colorRGB(255, 255, 0)

DIYables_TFT_RM68140_Shield TFT_display;

void setup() {
  Serial.begin(9600);
  TFT_display.begin();
  TFT_display.setRotation(1); // Landscape: 480x320

  // Draw various shapes and text so you can verify they remain the same after on/off
  TFT_display.fillScreen(BLUE);

  // Draw filled rectangle
  TFT_display.fillRect(20, 20, 120, 80, RED);

  // Draw outlined rectangle
  TFT_display.drawRect(160, 20, 120, 80, WHITE);

  // Draw filled circle
  TFT_display.fillCircle(370, 60, 40, GREEN);

  // Draw a horizontal line
  TFT_display.drawFastHLine(20, 130, 440, YELLOW);

  // Draw text
  TFT_display.setTextColor(WHITE);
  TFT_display.setTextSize(3);
  TFT_display.setCursor(80, 150);
  TFT_display.print("Hello DIYables");

  // Draw small text at bottom
  TFT_display.setTextSize(2);
  TFT_display.setCursor(40, 250);
  TFT_display.print("Display will turn off/on...");

  Serial.println("Display is ON. Content drawn.");
  Serial.println("Verify that the content remains the same after each on/off cycle.");
}

void loop() {
  delay(3000);

  // Screen goes white/blank (backlight stays on)
  TFT_display.turnOff();
  Serial.println("Display is OFF (screen is white/blank, backlight still on)");

  delay(3000);

  // Previous content (shapes and text) is restored automatically
  TFT_display.turnOn();
  Serial.println("Display is ON (content restored)");
}
