/*
  Touch Draw Lines Example
  ------------------------
  Draws lines on the screen following the pen.

  - Touch and drag on the screen to draw.
  - Lift the pen to stop drawing.
  - Touch again to start a new line from the last point.

  Make sure you have calibrated your touch screen and set the calibration values!
  
  Provided by DIYables

  This example code is in the public domain

  Product page:
  - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
  - https://www.amazon.com/dp/B0DQ3NQ3LW
*/

#include <DIYables_TFT_Touch_Shield.h>

#define RED DIYables_TFT::colorRGB(255, 0, 0)
#define WHITE DIYables_TFT::colorRGB(255, 255, 255)

// Set your calibration values here!
#define MIN_X 121
#define MAX_X 913
#define MIN_Y 78
#define MAX_Y 931

DIYables_TFT_ILI9488_Shield TFT_display;

void setup() {
    TFT_display.begin();
    TFT_display.setRotation(0);
    TFT_display.setTouchCalibration(MIN_X, MAX_X, MIN_Y, MAX_Y);
    TFT_display.fillScreen(WHITE);
}

void loop() {
    static int lastX = -1, lastY = -1;
    int x, y;

    if (TFT_display.getTouch(x, y)) {
        if (lastX >= 0 && lastY >= 0) {
            TFT_display.drawLine(lastX, lastY, x, y, RED);
        }
        lastX = x;
        lastY = y;
        delay(10); // Smoother drawing
    } else {
        lastX = -1;
        lastY = -1;
    }
}