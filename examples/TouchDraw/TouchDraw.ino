/*
  Touch Draw Lines Example
  ------------------------
  Draws lines on the screen following the pen.

  - Touch and drag on the screen to draw.
  - Lift the pen to stop drawing.
  - Touch again to start a new line from the last point.

  The touch screen works with default calibration values.
  Just in case the touch does not work properly, run the TouchCalibration example.
  
  Provided by DIYables

  This example code is in the public domain

  Product page:
  - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
  - https://www.amazon.com/dp/B0DQ3NQ3LW
*/

#include <DIYables_TFT_Touch_Shield.h>

#define RED DIYables_TFT::colorRGB(255, 0, 0)
#define WHITE DIYables_TFT::colorRGB(255, 255, 255)

// (Optional) Calibration values. Just in case touch does not work properly,
// run the TouchCalibration example and update the values below.
#define LEFT_X 136
#define RIGHT_X 907
#define TOP_Y 942
#define BOT_Y 139

DIYables_TFT_RM68140_Shield TFT_display;

#define PEN_RADIUS 3 // Radius (in pixels) of the circle drawn at each touch point

void setup() {
    TFT_display.begin();
    TFT_display.setRotation(0);
    TFT_display.setTouchCalibration(LEFT_X, RIGHT_X, TOP_Y, BOT_Y);
    TFT_display.fillScreen(WHITE);
}

void loop() {
    int x, y;

    if (TFT_display.getTouch(x, y)) {
        TFT_display.fillCircle(x, y, PEN_RADIUS, RED);
    }
}