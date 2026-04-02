/*
   Created by DIYables

   This example code is in the public domain

   Product page:
   - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
   - https://www.amazon.com/dp/B0DQ3NQ3LW
*/

#include <DIYables_TFT_Touch_Shield.h>

#define MAGENTA   DIYables_TFT::colorRGB(255, 0, 255)
#define WHITE     DIYables_TFT::colorRGB(255, 255, 255)

// For Arduino Uno R3, Uno R4, Mega, Due, Giga, DIYables STEM V3/V4 (default touch pins: XP=6, YP=A1, XM=A2, YM=7)
DIYables_TFT_RM68140_Shield TFT_display;
// For DIYables ESP32-S3 Uno-form factor (https://diyables.io/esp32-s3-uno), touch pins: XP=3, YP=1, XM=7, YM=14
// DIYables_TFT_RM68140_Shield TFT_display(3, 1, 7, 14);

void setup() {

  Serial.println(F("Arduino TFT Touch LCD Display - show text and float number"));

  TFT_display.begin();

  // Set the rotation (0 to 3)
  TFT_display.setRotation(1);  // Rotate screen 90 degrees
  TFT_display.fillScreen(WHITE);

  // Set text color and size
  TFT_display.setTextColor(MAGENTA);
  TFT_display.setTextSize(3);  // Adjust text size as needed

  // Sample temperature value
  float temperature = 23.5;
  float humidity = 78.6;

  // Display temperature with degree symbol
  TFT_display.setCursor(20, 20);    // Set cursor position (x, y)
  TFT_display.print("Temperature: ");
  TFT_display.print(temperature, 1);  // Print temperature with 1 decimal place
  TFT_display.print(char(247));
  TFT_display.println("C");

  // Display humidity
  TFT_display.setCursor(20, 60);    // Set cursor position (x, y)
  TFT_display.print("Humidity: ");
  TFT_display.print(humidity, 1);   // Print humidity with 1 decimal place
  TFT_display.print("%");
}

void loop(void) {
}

