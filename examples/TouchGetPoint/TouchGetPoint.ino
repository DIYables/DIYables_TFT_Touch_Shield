/*
   Touch Get Point Example
   -----------------------
   This example demonstrates how to read and display touch coordinates using the DIYables TFT Touch Shield.

   When you touch the screen, the code prints the mapped (screen) X and Y coordinates to the Serial Monitor
   and draws a red dot at the touched location.

   **Note:**  
   The touch screen works with default calibration values.
   Just in case the touch does not work properly, run the `TouchCalibration` example
   and set the calibration values in the code using `setTouchCalibration()`.

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

// For Arduino Uno R3, Uno R4, Mega, Due, Giga, DIYables STEM V3/V4 (default touch pins: XP=6, YP=A1, XM=A2, YM=7)
DIYables_TFT_RM68140_Shield TFT_display;
// For DIYables ESP32-S3 Uno-form factor (https://diyables.io/esp32-s3-uno), touch pins: XP=3, YP=1, XM=7, YM=14
// DIYables_TFT_RM68140_Shield TFT_display(3, 1, 7, 14);

void setup() {
  Serial.begin(9600);
  TFT_display.begin();
  // Set the rotation (0 to 3)
  TFT_display.setRotation(0);

  TFT_display.fillScreen(WHITE);

  TFT_display.setTouchCalibration(LEFT_X, RIGHT_X, TOP_Y, BOT_Y);
  Serial.println("Touch the screen to see coordinates.");
}

void loop() {
  int x, y;
  if (TFT_display.getTouch(x, y)) {
    Serial.print("Touch at: ");
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);
    TFT_display.fillCircle(x, y, 4, RED);  // Draw a red dot where touched
    delay(200);                             // Debounce
  }
}
