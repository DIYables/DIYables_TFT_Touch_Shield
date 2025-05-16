/*
   Created by DIYables

   This example code is in the public domain

   Product page:
   - https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch
   - https://www.amazon.com/dp/B0DQ3NQ3LW
*/

#include <DIYables_TFT_Touch_Shield.h>

#define BLACK     DIYables_TFT::colorRGB(0, 0, 0)
#define BLUE      DIYables_TFT::colorRGB(0, 0, 255)
#define RED       DIYables_TFT::colorRGB(255, 0, 0)
#define GREEN     DIYables_TFT::colorRGB(0, 255, 0)
#define ORANGE    DIYables_TFT::colorRGB(255, 165, 0)
#define PINK      DIYables_TFT::colorRGB(255, 192, 203)
#define VIOLET    DIYables_TFT::colorRGB(148, 0, 211)
#define TURQUOISE DIYables_TFT::colorRGB(64, 224, 208)
#define WHITE     DIYables_TFT::colorRGB(255, 255, 255)

DIYables_TFT_ILI9488_Shield TFT_display;

// Helper to draw a filled diamond
void fillDiamond(int cx, int cy, int h, int v, uint16_t color) {
  int x0 = cx, y0 = cy - v;
  int x1 = cx + h, y1 = cy;
  int x2 = cx, y2 = cy + v;
  int x3 = cx - h, y3 = cy;
  TFT_display.fillTriangle(x0, y0, x1, y1, x2, y2, color);
  TFT_display.fillTriangle(x0, y0, x2, y2, x3, y3, color);
}

void setup() {
  TFT_display.begin();
  TFT_display.setRotation(1);  // Landscape: 480x320
}

void loop() {
  TFT_display.fillScreen(BLACK);

  // Outlined circle (top left)
  TFT_display.drawCircle(70, 60, 40, RED);

  // Filled circle (top center)
  TFT_display.fillCircle(180, 60, 40, RED);

  // Outlined triangle (top right)
  TFT_display.drawTriangle(260, 30, 340, 30, 300, 100, BLUE);

  // Filled triangle (top far right)
  TFT_display.fillTriangle(370, 30, 450, 30, 410, 100, GREEN);

  // Outlined rectangle (middle left)
  TFT_display.drawRect(30, 130, 80, 50, ORANGE);

  // Filled rectangle (middle center)
  TFT_display.fillRect(140, 130, 80, 50, TURQUOISE);

  // Outlined round rectangle (middle right)
  TFT_display.drawRoundRect(260, 130, 80, 50, 15, VIOLET);

  // Filled round rectangle (middle far right)
  TFT_display.fillRoundRect(370, 130, 80, 50, 15, PINK);

  // Outlined diamond shape (bottom left)
  int cx1 = 120, cy1 = 250, h1 = 40, v1 = 50;
  TFT_display.drawLine(cx1, cy1 - v1, cx1 + h1, cy1, GREEN);
  TFT_display.drawLine(cx1 + h1, cy1, cx1, cy1 + v1, GREEN);
  TFT_display.drawLine(cx1, cy1 + v1, cx1 - h1, cy1, GREEN);
  TFT_display.drawLine(cx1 - h1, cy1, cx1, cy1 - v1, GREEN);

  // Filled diamond shape (bottom right)
  int cx2 = 355, cy2 = 250, h2 = 40, v2 = 50;
  fillDiamond(cx2, cy2, h2, v2, BLUE);

  delay(10000);
}