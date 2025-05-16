#include <DIYables_TFT_Shield.h>
#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_ILI9488_Shield::DIYables_TFT_ILI9488_Shield()
  : DIYables_TFT_ILI9486_Shield() {}

void DIYables_TFT_ILI9488_Shield::begin() {
    DIYables_TFT_ILI9486_Shield::begin();
    DIYables_TFT_ILI9486_Shield::invertDisplay(true);
}

void DIYables_TFT_ILI9488_Shield::invertDisplay(bool i) {
   // ILI9488 is inverted contrary to ILI9486
   // Invert the display if i is true
    if(i)
        DIYables_TFT_ILI9486_Shield::invertDisplay(false);
    else
        DIYables_TFT_ILI9486_Shield::invertDisplay(true);
}


// --- Touch functions for DIYables_TFT_ILI9488_Shield ---

void DIYables_TFT_ILI9488_Shield::setTouchCalibration(int min_x, int max_x, int min_y, int max_y) {
    touch_min_x = min_x;
    touch_max_x = max_x;
    touch_min_y = min_y;
    touch_max_y = max_y;
}

#define NUMSAMPLES 10

static void insert_sort(int array[], uint8_t size) {
    for (uint8_t i = 1; i < size; i++) {
        int temp = array[i];
        int j = i;
        while (j > 0 && array[j - 1] > temp) {
            array[j] = array[j - 1];
            j--;
        }
        array[j] = temp;
    }
}

void DIYables_TFT_ILI9488_Shield::readTouchRaw(int &x, int &y, int &z) {
    int samples[NUMSAMPLES];

    // --- Read X ---
    pinMode(YP, INPUT);
    pinMode(YM, INPUT);
    pinMode(XM, OUTPUT);
    pinMode(XP, OUTPUT);
    digitalWrite(XM, LOW);
    digitalWrite(XP, HIGH);
    delayMicroseconds(20);
    for (uint8_t i = 0; i < NUMSAMPLES; i++) {
        samples[i] = 1023 - analogRead(YP);
        delayMicroseconds(10);
    }
    insert_sort(samples, NUMSAMPLES);
    // Average the middle 4 samples (remove 3 lowest and 3 highest)
    int sum_x = 0;
    for (uint8_t i = 3; i < 7; i++) {
        sum_x += samples[i];
    }
    int avg_x = sum_x / 4;
    x = (avg_x < 10) ? -1 : avg_x;

    // --- Read Y ---
    pinMode(XP, INPUT);
    pinMode(XM, INPUT);
    pinMode(YP, OUTPUT);
    pinMode(YM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(YM, LOW);
    delayMicroseconds(20);
    for (uint8_t i = 0; i < NUMSAMPLES; i++) {
        samples[i] = 1023 - analogRead(XM);
        delayMicroseconds(10);
    }
    insert_sort(samples, NUMSAMPLES);
    int sum_y = 0;
    for (uint8_t i = 3; i < 7; i++) {
        sum_y += samples[i];
    }
    int avg_y = sum_y / 4;
    y = (avg_y < 10) ? -1 : avg_y;

    // --- Read Z (pressure) ---
    pinMode(XP, OUTPUT);
    digitalWrite(XP, LOW);
    pinMode(YM, OUTPUT);
    digitalWrite(YM, HIGH);
    pinMode(YP, INPUT);
    pinMode(XM, INPUT);
    int z1 = analogRead(XM);
    int z2 = analogRead(YP);
    z = z2 - z1;

	SET_DATA_DIR_OUT();
	SET_CONTROL_DIR_OUT();

	#ifndef ARDUINO_API_USED
	PIN_HIGH(RD_PORT, RD_PIN);
	#else
	PIN_HIGH(API_PIN_RD);
	#endif


	#ifndef ARDUINO_API_USED
	PIN_LOW(CS_PORT, CS_PIN);
	#else
	PIN_LOW(API_PIN_CS);
	#endif
}

bool DIYables_TFT_ILI9488_Shield::getTouch(int &screenX, int &screenY) {
    int raw_x, raw_y, z;
    readTouchRaw(raw_x, raw_y, z);
    if (raw_x < 0 || raw_y < 0) return false;
    if (z > 50 && z < 1000) {
        // Map raw values to screen coordinates using calibration values
        int x = map(raw_x, touch_min_x, touch_max_x, 0, width() - 1);
        int y = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
        // Clamp to screen bounds
        x = constrain(x, 0, width() - 1);
        y = constrain(y, 0, height() - 1);

        // Adjust for rotation
        switch (getRotation()) {
            case 0:
                screenX = x;
                screenY = y;
                break;
            case 1:
                screenX = y;
                screenY = width() - 1 - x;
                break;
            case 2:
                screenX = width() - 1 - x;
                screenY = height() - 1 - y;
                break;
            case 3:
                screenX = height() - 1 - y;
                screenY = x;
                break;
            default:
                screenX = x;
                screenY = y;
                break;
        }
        return true;
    }
    return false;
}
