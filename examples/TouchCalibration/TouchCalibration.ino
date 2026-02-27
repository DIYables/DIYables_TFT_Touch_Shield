/*
  Touch Screen Calibration Example
  --------------------------------
  The touch screen works with default calibration values.
  Just in case the touch does not work properly, run this calibration example.

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
       along with the current calibration values.
     - Wait for about a second until the dot moves to the next corner.

  7. Repeat for all four corners.

  8. After all corners are done, the Serial Monitor will print:
       - The final LEFT, RIGHT, TOP, BOTTOM calibration values.
       - A ready-to-use line for your code:
         setTouchCalibration(LEFT, RIGHT, TOP, BOTTOM);

  9. Copy the printed calibration line and paste it into your main sketch,
     before using any touch functions.

  10. You only need to calibrate once for your screen, unless you change hardware.

  If you have any trouble, make sure your Serial Monitor is open and set to 9600 baud,
  and that you are touching the blinking dot as it appears in each corner.

  Happy calibrating!
*/

#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_RM68140_Shield TFT_display;

// Store raw readings for each corner
// Corner order: 0=top-left, 1=top-right, 2=bottom-right, 3=bottom-left
int corner_x[4] = {0, 0, 0, 0};
int corner_y[4] = {0, 0, 0, 0};

const int corners[4][2] = {
    {20, 20},                                           // Top-left
    {TFT_display.width() - 21, 20},                     // Top-right
    {TFT_display.width() - 21, TFT_display.height() - 21}, // Bottom-right
    {20, TFT_display.height() - 21}                     // Bottom-left
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

    if (z > 200 && z < 1000) {
        Serial.print("Corner ");
        Serial.print(corner + 1);
        Serial.print(": Raw X: ");
        Serial.print(raw_x);
        Serial.print("  Raw Y: ");
        Serial.print(raw_y);
        Serial.print("  Pressure: ");
        Serial.println(z);

        // Store raw values for this corner
        corner_x[corner] = raw_x;
        corner_y[corner] = raw_y;

        delay(1000); // Wait for user to release before next corner

        // Move to next corner
        TFT_display.fillCircle(corners[corner][0], corners[corner][1], 10, 0xFFFF); // Erase dot
        corner++;
        if (corner >= 4) {
            // Compute calibration values:
            // LEFT  = average raw X of the two left corners (top-left, bottom-left)
            // RIGHT = average raw X of the two right corners (top-right, bottom-right)
            // TOP   = average raw Y of the two top corners (top-left, top-right)
            // BOT   = average raw Y of the two bottom corners (bottom-right, bottom-left)
            int left_x  = (corner_x[0] + corner_x[3]) / 2;
            int right_x = (corner_x[1] + corner_x[2]) / 2;
            int top_y   = (corner_y[0] + corner_y[1]) / 2;
            int bot_y   = (corner_y[2] + corner_y[3]) / 2;

            Serial.println("\nCalibration complete!");
            Serial.print("LEFT = "); Serial.println(left_x);
            Serial.print("RIGHT = "); Serial.println(right_x);
            Serial.print("TOP = "); Serial.println(top_y);
            Serial.print("BOTTOM = "); Serial.println(bot_y);
            Serial.println();
            Serial.print("Use this in your code: setTouchCalibration(");
            Serial.print(left_x); Serial.print(", ");
            Serial.print(right_x); Serial.print(", ");
            Serial.print(top_y); Serial.print(", ");
            Serial.print(bot_y); Serial.println(");");
            while (1); // Stop here
        }
        delay(500);
    }
}