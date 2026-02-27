/*
  Touch Button Press/Release Example
  ----------------------------------
  This example shows how to detect press and release events on a rectangular button
  using the DIYables TFT Touch Shield. When you touch inside the button, it changes color
  and displays "PRESSED". When you release, it returns to its original state.

  The touch screen works with default calibration values.
  Just in case the touch does not work properly, run the TouchCalibration example.
  
  Provided by DIYables

  This example code is in the public domain

  Product page:
  - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
  - https://www.amazon.com/dp/B0DQ3NQ3LW
*/

#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_RM68140_Shield TFT_display;

#define BLACK DIYables_TFT::colorRGB(0, 0, 0)
#define WHITE DIYables_TFT::colorRGB(255, 255, 255)
#define GRAY DIYables_TFT::colorRGB(128, 128, 128)
#define RED DIYables_TFT::colorRGB(255, 0, 0)

// (Optional) Calibration values. Just in case touch does not work properly,
// run the TouchCalibration example and update the values below.
#define LEFT_X 136
#define RIGHT_X 907
#define TOP_Y 942
#define BOT_Y 139

#define BUTTON_X      70
#define BUTTON_Y      100
#define BUTTON_W      180
#define BUTTON_H      60

bool lastPressed = false;

void setup() {
    Serial.begin(9600);
    TFT_display.begin();
    TFT_display.setRotation(0);
    TFT_display.setTouchCalibration(LEFT_X, RIGHT_X, TOP_Y, BOT_Y);
    TFT_display.fillScreen(WHITE); // White background

    // Draw button
    TFT_display.fillRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, RED); 
    TFT_display.drawRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, BLACK);
    TFT_display.setTextColor(WHITE);
    TFT_display.setTextSize(3);
    TFT_display.setCursor(BUTTON_X + 30, BUTTON_Y + 18);
    TFT_display.print("PRESS");
}

void loop() {
    int x, y;
    bool pressed = false;
    if (TFT_display.getTouch(x, y)) {
        if (x >= BUTTON_X && x < (BUTTON_X + BUTTON_W) && y >= BUTTON_Y && y < (BUTTON_Y + BUTTON_H)) {
            pressed = true;
        }
    }

    if(lastPressed == pressed) {
        // No change in state, do nothing
        return;
    }

    // Detect press event
    if (pressed && !lastPressed) {
        // Just pressed
        Serial.println("Button PRESSED");
        TFT_display.drawRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, BLACK);
        TFT_display.fillRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, GRAY);
        TFT_display.setTextColor(BLACK);
        TFT_display.setCursor(BUTTON_X + 30, BUTTON_Y + 18);
        TFT_display.print("PRESSED");
    }

    // Detect release event
    if (!pressed && lastPressed) {
        // Just released
        Serial.println("Button RELEASED");
        TFT_display.drawRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, BLACK);
        TFT_display.fillRect(BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, RED);
        TFT_display.setTextColor(WHITE);
        TFT_display.setCursor(BUTTON_X + 30, BUTTON_Y + 18);
        TFT_display.print("PRESS");
    }

    lastPressed = pressed;
}