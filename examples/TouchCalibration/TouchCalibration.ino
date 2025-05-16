/*
  Touch Screen Calibration Example
  --------------------------------
  This example helps you calibrate your DIYables TFT Touch Shield.

  INSTRUCTIONS:

  1. Open the Arduino IDE.

  2. Go to: File > Examples > DIYables_TFT_Touch_Shield > TouchCalibration
     (If you don't see this, make sure the library is installed correctly.)

  3. Upload the sketch to your Arduino board.

  4. Open the Serial Monitor (Ctrl+Shift+M) in the Arduino IDE.
     Make sure the baud rate is set to 9600.

  5. The screen will show a blinking red dot in each corner, one at a time.
     The dot will also display a number (1–4) indicating the order:
       1. Top-left
       2. Top-right
       3. Bottom-right
       4. Bottom-left

  6. When the dot blinks in a corner, touch and hold that corner on the screen.
     - The Serial Monitor will print the raw X, Y, and pressure values for that corner,
       along with the current min_x, max_x, min_y, and max_y values.
     - Wait for about a second until the dot moves to the next corner.

  7. Repeat for all four corners.

  8. After all corners are done, the Serial Monitor will print:
       - The final min_x, max_x, min_y, max_y values.
       - A ready-to-use line for your code:
         setTouchCalibration(min_x, max_x, min_y, max_y);

  9. Copy the printed calibration line and paste it into your main sketch,
     before using any touch functions.

  10. You only need to calibrate once for your screen, unless you change hardware.

  If you have any trouble, make sure your Serial Monitor is open and set to 9600 baud,
  and that you are touching the blinking dot as it appears in each corner.

  Happy calibrating!
*/

#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_ILI9488_Shield TFT_display;

int min_x = 1023, max_x = 0, min_y = 1023, max_y = 0;

const int corners[4][2] = {
    {20, 20},                       // Top-left
    {TFT_display.width() - 21, 20},         // Top-right
    {TFT_display.width() - 21, TFT_display.height() - 21}, // Bottom-right
    {20, TFT_display.height() - 21}         // Bottom-left
};

void setup() {
    Serial.begin(9600);
    TFT_display.begin();
    TFT_display.setRotation(0); // Set orientation to 0
    TFT_display.fillScreen(0xFFFF);
    Serial.println("Touch the blinking dot in each corner (1-4).");
}

void loop() {
    static int corner = 0;
    static unsigned long lastBlink = 0;
    static bool dotOn = false;

    // Blink the indicator dot
    if (millis() - lastBlink > 400) {
        lastBlink = millis();
        dotOn = !dotOn;
        TFT_display.fillCircle(corners[corner][0], corners[corner][1], 10, dotOn ? 0xF800 : 0xFFFF);
        TFT_display.setTextColor(0x0000, 0xFFFF);
        TFT_display.setTextSize(2);
        TFT_display.setCursor(corners[corner][0] + 15, corners[corner][1] - 10);
        TFT_display.print(corner + 1); // Print corner number
    }

    int raw_x, raw_y, z;
    TFT_display.readTouchRaw(raw_x, raw_y, z);

    if (raw_x >= 0 && raw_y >= 0 && z > 10 && z < 1000) {
        Serial.print("Corner ");
        Serial.print(corner + 1);
        Serial.print(": Raw X: ");
        Serial.print(raw_x);
        Serial.print("  Raw Y: ");
        Serial.print(raw_y);
        Serial.print("  Pressure: ");
        Serial.println(z);

        // Update min/max for this touch
        if (raw_x < min_x) min_x = raw_x;
        if (raw_x > max_x) max_x = raw_x;
        if (raw_y < min_y) min_y = raw_y;
        if (raw_y > max_y) max_y = raw_y;

        Serial.print("min_x = "); Serial.println(min_x);
        Serial.print("max_x = "); Serial.println(max_x);
        Serial.print("min_y = "); Serial.println(min_y);
        Serial.print("max_y = "); Serial.println(max_y);

        delay(1000); // Wait for user to release before next corner

        // Move to next corner
        TFT_display.fillCircle(corners[corner][0], corners[corner][1], 10, 0xFFFF); // Erase dot
        corner++;
        if (corner >= 4) {
            Serial.println("Calibration complete!");
            Serial.print("Use these values: setTouchCalibration(");
            Serial.print("min_x = "); Serial.print(min_x); Serial.print(", ");
            Serial.print("max_x = "); Serial.print(max_x); Serial.print(", ");
            Serial.print("min_y = "); Serial.print(min_y); Serial.print(", ");
            Serial.print("max_y = "); Serial.print(max_y); Serial.println(");");
            while (1); // Stop here
        }
        delay(500);
    }
}