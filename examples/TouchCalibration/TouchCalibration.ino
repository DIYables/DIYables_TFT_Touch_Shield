/*
  Touch Screen Calibration Example
  ---------------------------------
  This example measures the raw touch coordinates at all four screen corners
  and prints ready-to-use calibration values to the Serial Monitor.

  It uses readTouchRaw() directly — it does NOT rely on getTouch() or any
  existing calibration values, so it works even when touch is completely broken.

  INSTRUCTIONS:

  1. Upload this sketch to your board.

  2. Open the Serial Monitor (Ctrl+Shift+M) and set baud rate to 9600.

  3. The screen shows a blinking red dot in each corner, numbered 1–4:
       1 = Top-left
       2 = Top-right
       3 = Bottom-right
       4 = Bottom-left

  4. Press and HOLD firmly on the blinking dot.
     Keep holding until the Serial Monitor prints "Captured!" for that corner.

  5. Release, then wait for the next dot to appear and repeat.

  6. After all 4 corners, the Serial Monitor prints the calibration values and
     a ready-to-use setTouchCalibration() line. Copy it into your sketch.

  NOTE: While waiting, the Serial Monitor continuously prints the live raw
  Z/X/Y readings so you can confirm that touch is being detected.

  Provided by DIYables

  This example code is in the public domain
*/

#include <DIYables_TFT_Touch_Shield.h>

// For Arduino Uno R3, Uno R4, Mega, Due, Giga, DIYables STEM V3/V4 (default touch pins: XP=6, YP=A1, XM=A2, YM=7)
DIYables_TFT_RM68140_Shield TFT_display;
// For DIYables ESP32-S3 Uno-form factor (https://diyables.io/esp32-s3-uno), touch pins: XP=3, YP=1, XM=7, YM=14
// DIYables_TFT_RM68140_Shield TFT_display(3, 1, 7, 14);

// Minimum pressure to count as a valid touch. No upper limit —
// the raw Z value varies widely with touch pressure and board type.
#define TOUCH_Z_MIN   10

// How many consecutive valid samples are required before a corner is accepted.
// This rejects accidental grazes and noisy single readings.
#define SAMPLES_NEEDED  10

// Delay between samples (ms).
#define SAMPLE_DELAY_MS 30

#define DOT_RADIUS  12

// Corner pixel positions (filled in setup once display size is known).
// Order: 0=top-left, 1=top-right, 2=bottom-right, 3=bottom-left
int cx[4], cy[4];

// Captured averaged raw values per corner.
int cap_x[4], cap_y[4];

// -----------------------------------------------------------------------

void drawDot(int corner, bool on) {
    uint16_t color = on ? 0xF800 /* red */ : 0xFFFF /* white */;
    TFT_display.fillCircle(cx[corner], cy[corner], DOT_RADIUS, color);
    TFT_display.setTextSize(2);
    TFT_display.setTextColor(0x0000, color);
    TFT_display.setCursor(cx[corner] - 6, cy[corner] - 8);
    TFT_display.print(corner + 1);
}

// Blocks until a stable touch is detected at the given corner.
void captureCorner(int corner) {
    const char* names[] = { "Top-left", "Top-right", "Bottom-right", "Bottom-left" };

    Serial.println();
    Serial.print("Corner "); Serial.print(corner + 1);
    Serial.print(" ("); Serial.print(names[corner]); Serial.println(")");
    Serial.println("  Press and HOLD firmly on the blinking dot.");
    Serial.println("  Keep holding until you see 'Captured!'");

    unsigned long lastBlink = 0;
    unsigned long lastPrint = 0;
    bool dotOn = false;

    int goodSamples = 0;
    long sumX = 0, sumY = 0;

    while (true) {
        // Blink the dot
        if (millis() - lastBlink > 400) {
            lastBlink = millis();
            dotOn = !dotOn;
            drawDot(corner, dotOn);
        }

        int raw_x, raw_y, z;
        TFT_display.readTouchRaw(raw_x, raw_y, z);

        // Print live status so the user can verify touch is being detected
        if (millis() - lastPrint > 500) {
            lastPrint = millis();
            Serial.print("  Z="); Serial.print(z);
            Serial.print("  X="); Serial.print(raw_x);
            Serial.print("  Y="); Serial.println(raw_y);
        }

        if (z > TOUCH_Z_MIN) {
            sumX += raw_x;
            sumY += raw_y;
            goodSamples++;

            if (goodSamples >= SAMPLES_NEEDED) {
                // Accept: store averaged values
                cap_x[corner] = (int)(sumX / goodSamples);
                cap_y[corner] = (int)(sumY / goodSamples);

                Serial.print("  Captured!  avg X="); Serial.print(cap_x[corner]);
                Serial.print("  avg Y="); Serial.println(cap_y[corner]);

                drawDot(corner, false);    // erase dot
                delay(900);               // let user release finger
                return;
            }
        } else {
            // Touch lost — reset accumulator and require a fresh run
            if (goodSamples > 0) {
                goodSamples = 0;
                sumX = 0;
                sumY = 0;
            }
        }

        delay(SAMPLE_DELAY_MS);
    }
}

// -----------------------------------------------------------------------

void setup() {
    Serial.begin(9600);
    TFT_display.begin();
    TFT_display.setRotation(0);
    TFT_display.fillScreen(0xFFFF);

    // Compute corner pixel positions from actual display size
    int w = TFT_display.width();
    int h = TFT_display.height();
    int m = DOT_RADIUS + 6;     // margin from edge
    cx[0] = m;     cy[0] = m;
    cx[1] = w - m; cy[1] = m;
    cx[2] = w - m; cy[2] = h - m;
    cx[3] = m;     cy[3] = h - m;

    Serial.println("=== Touch Screen Calibration ===");
    Serial.println("A blinking dot appears in each corner in order 1 to 4.");
    Serial.println("Press and HOLD each dot until the Serial Monitor shows 'Captured!'");
    Serial.println("The live Z/X/Y readings are printed every 0.5 s so you can");
    Serial.println("verify that touch is being detected.");
    Serial.println();
}

void loop() {
    static int corner = 0;

    if (corner < 4) {
        captureCorner(corner);
        corner++;
        return;
    }

    // All 4 corners captured — compute calibration values
    int left_x  = (cap_x[0] + cap_x[3]) / 2;
    int right_x = (cap_x[1] + cap_x[2]) / 2;
    int top_y   = (cap_y[0] + cap_y[1]) / 2;
    int bot_y   = (cap_y[2] + cap_y[3]) / 2;

    Serial.println();
    Serial.println("=== Calibration Complete! ===");
    Serial.println("Update your sketch with these values:");
    Serial.println();
    Serial.print("  #define LEFT_X  "); Serial.println(left_x);
    Serial.print("  #define RIGHT_X "); Serial.println(right_x);
    Serial.print("  #define TOP_Y   "); Serial.println(top_y);
    Serial.print("  #define BOT_Y   "); Serial.println(bot_y);
    Serial.println();
    Serial.println("And paste this line into setup() before using getTouch():");
    Serial.print("  TFT_display.setTouchCalibration(");
    Serial.print(left_x); Serial.print(", ");
    Serial.print(right_x); Serial.print(", ");
    Serial.print(top_y); Serial.print(", ");
    Serial.print(bot_y); Serial.println(");");

    // Show result on screen
    TFT_display.fillScreen(0xFFFF);
    TFT_display.setTextColor(0x0000);
    TFT_display.setTextSize(2);
    TFT_display.setCursor(10, 20);
    TFT_display.print("Done!");
    TFT_display.setCursor(10, 50);
    TFT_display.print("See Serial Monitor");
    TFT_display.setCursor(10, 80);
    TFT_display.print("for values.");

    while (1);  // stop
}