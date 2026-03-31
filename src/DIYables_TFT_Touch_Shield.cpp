#include <DIYables_TFT_Shield.h>
#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield()
  : DIYables_TFT_ILI9486_Shield(), _ts(TS_XP, TS_YP, TS_XM, TS_YM, 300),
    _ts_xp(TS_XP), _ts_yp(TS_YP), _ts_xm(TS_XM), _ts_ym(TS_YM) {}

DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield(
  uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
  uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
  uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
  uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym)
  : DIYables_TFT_ILI9486_Shield(d0, d1, d2, d3, d4, d5, d6, d7, rd, wr, cd, cs, rst),
    _ts(ts_xp, ts_yp, ts_xm, ts_ym, 300),
    _ts_xp(ts_xp), _ts_yp(ts_yp), _ts_xm(ts_xm), _ts_ym(ts_ym) {}

void DIYables_TFT_RM68140_Shield::begin() {
    DIYables_TFT_ILI9486_Shield::begin();

    // The parent init sends ILI9486-specific registers (0xD0, 0xD1, 0xC8) that
    // the RM68140 does not recognise.  Because the RM68140 may interpret these
    // unknown registers with a different parameter count, subsequent commands
    // (including 0x3A pixel format) can be swallowed as stale data, leaving the
    // controller in an undefined colour mode.
    //
    // Fix: issue a software reset to clear ALL register state, then re-send
    // only the registers the RM68140 actually needs.

    writeCommand(0x01);   // Software Reset — clears all registers to defaults
    delay(120);        // Wait for reset to complete

    writeCommand(0x11);   // Sleep Out
    delay(120);

    writeCommand(0x3A);   // Interface Pixel Format
    writeData(0x55); // 16-bit/pixel (RGB565)

    // Power Control
    writeCommand(0xC0);
    writeData(0x0D); writeData(0x0D);
    writeCommand(0xC1);
    writeData(0x43); writeData(0x00);
    writeCommand(0xC2);
    writeData(0x00);

    // VCOM Control — affects colour balance and contrast
    writeCommand(0xC5);
    writeData(0x00); writeData(0x48);
    writeData(0x00); writeData(0x48);

    // Display Function Control
    writeCommand(0xB6);
    writeData(0x00);
    writeData(0x22); // SS=1, GS=0, REV=1
    writeData(0x3B); // NL = 480 lines

    // Positive Gamma Correction
    writeCommand(0xE0);
    writeData(0x0F); writeData(0x21); writeData(0x1C); writeData(0x0B);
    writeData(0x0E); writeData(0x08); writeData(0x49); writeData(0x98);
    writeData(0x38); writeData(0x09); writeData(0x11); writeData(0x03);
    writeData(0x14); writeData(0x10); writeData(0x00);

    // Negative Gamma Correction
    writeCommand(0xE1);
    writeData(0x0F); writeData(0x2F); writeData(0x2B); writeData(0x0C);
    writeData(0x0E); writeData(0x06); writeData(0x47); writeData(0x76);
    writeData(0x37); writeData(0x07); writeData(0x11); writeData(0x04);
    writeData(0x23); writeData(0x1E); writeData(0x00);

    writeCommand(0x13);   // Normal Display Mode ON
    writeCommand(0x29);   // Display ON
    delay(25);

    setRotation(0);    // Sets MADCTL (0x36) + Display Function Control (0xB6)
}

void DIYables_TFT_RM68140_Shield::invertDisplay(bool i) {
   // ILI9488 is inverted contrary to ILI9486
   // Invert the display if i is true
    if(i)
        DIYables_TFT_ILI9486_Shield::invertDisplay(false);
    else
        DIYables_TFT_ILI9486_Shield::invertDisplay(true);
}

// Turn the display back on after turnOff(). The previous content is restored.
void DIYables_TFT_RM68140_Shield::turnOn() {
  writeCommand(0x00); // NOP - ensure not in data mode
  writeCommand(0x11); // Sleep Out
  delay(120);      // Wait for sleep out to complete
  writeCommand(0x29); // Display ON
  delay(20);
}

// Turn off the display content. The screen goes white/blank, but the backlight
// remains on because it is hardwired to the power rail on the shield PCB and
// cannot be controlled by software. The frame buffer content is preserved, so
// calling turnOn() will restore the previous display content without redrawing.
void DIYables_TFT_RM68140_Shield::turnOff() {
  writeCommand(0x00); // NOP - ensure not in data mode
  writeCommand(0x28); // Display OFF
  delay(20);
  writeCommand(0x10); // Sleep In (low power mode)
  delay(120);      // Wait for sleep in to complete
}

// --- Rotation fix for RM68140-based display ---
// The RM68140 handles MX/MY (mirror) through register 0xB6 (Display Function
// Control) as GS/SS bits, NOT through MADCTL (0x36). Putting MX/MY in MADCTL
// causes mirrored display on rotations 1 and 2.
// Reference: MCUFRIEND_kbv handling for LCD ID 0x6814.

void DIYables_TFT_RM68140_Shield::setRotation(uint8_t r) {
  Adafruit_GFX::setRotation(r);

  uint8_t val;
  switch (r & 3) {
    case 0:  val = 0x48; break; // Portrait:       MX | BGR
    case 1:  val = 0x28; break; // Landscape:       MV | BGR
    case 2:  val = 0x98; break; // Portrait 180:   MY | ML | BGR
    default: val = 0xF8; break; // Landscape 270:  MY | MX | MV | ML | BGR
  }

  // Move MY -> GS (bit6) and MX -> SS (bit5) into Display Function Control
  uint8_t GS = (val & 0x80) ? (1 << 6) : 0;  // MY -> GS
  uint8_t SS = (val & 0x40) ? (1 << 5) : 0;   // MX -> SS
  val &= 0x28;  // Keep only MV and BGR in MADCTL

  // Display Function Control (0xB6): set scan direction
  writeCommand(0xB6);
  writeData(0x00);
  writeData(GS | SS | 0x02);  // GS, SS, REV=1
  writeData(0x3B);            // NL = 480 lines

  // MADCTL (0x36): only MV and BGR (no MX/MY)
  writeCommand(0x36);
  writeData(val);
}

// --- Touch functions for DIYables_TFT_RM68140_Shield (RM68140 driver) ---
// Uses the Adafruit TouchScreen library (same approach as MCUFRIEND_kbv Touch_shield_new)

void DIYables_TFT_RM68140_Shield::setTouchCalibration(int min_x, int max_x, int min_y, int max_y) {
    touch_min_x = min_x;
    touch_max_x = max_x;
    touch_min_y = min_y;
    touch_max_y = max_y;
}

void DIYables_TFT_RM68140_Shield::readTouchRaw(int &x, int &y, int &z) {
    TSPoint tp = _ts.getPoint();

    // Restore shared pins for TFT operation.
    // TouchScreen.getPoint() leaves YP and XM in INPUT (analog) mode,
    // but these pins are shared with the TFT data/control bus.
    pinMode(_ts_yp, OUTPUT);
    pinMode(_ts_xm, OUTPUT);

    // Restore all TFT data bus and control pin directions
    #ifndef ARDUINO_API_USED
    if (!_useAPI) {
        SET_DATA_DIR_OUT();
        SET_CONTROL_DIR_OUT();
        PIN_HIGH(RD_PORT, RD_PIN);
        PIN_LOW(CS_PORT, CS_PIN);
    } else
    #endif
    {
        for (uint8_t i = 0; i < 8; i++) { pinMode(_d[i], OUTPUT); }
        pinMode(_rd, OUTPUT);
        pinMode(_wr, OUTPUT);
        pinMode(_cd, OUTPUT);
        pinMode(_cs, OUTPUT);
        pinMode(_rst, OUTPUT);
        digitalWrite(_rd, HIGH);
        digitalWrite(_cs, LOW);
    }

    x = tp.x;
    y = tp.y;
    z = tp.z;
}

bool DIYables_TFT_RM68140_Shield::getTouch(int &screenX, int &screenY) {
    int raw_x, raw_y, z;
    readTouchRaw(raw_x, raw_y, z);

    if (z > 200 && z < 1000) {
        // Calibration values are for PORTRAIT orientation (rotation 0).
        // tp.y is always the long dimension of the touch panel.
        // Map to the current pixel orientation using the same approach as MCUFRIEND_kbv.
        switch (getRotation()) {
            case 0:
                screenX = map(raw_x, touch_max_x, touch_min_x, 0, width() - 1);
                screenY = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
                break;
            case 1:
                screenX = map(raw_y, touch_min_y, touch_max_y, 0, width() - 1);
                screenY = map(raw_x, touch_min_x, touch_max_x, 0, height() - 1);
                break;
            case 2:
                screenX = map(raw_x, touch_min_x, touch_max_x, 0, width() - 1);
                screenY = map(raw_y, touch_max_y, touch_min_y, 0, height() - 1);
                break;
            case 3:
                screenX = map(raw_y, touch_max_y, touch_min_y, 0, width() - 1);
                screenY = map(raw_x, touch_max_x, touch_min_x, 0, height() - 1);
                break;
            default:
                screenX = map(raw_x, touch_max_x, touch_min_x, 0, width() - 1);
                screenY = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
                break;
        }

        // Clamp to screen bounds
        screenX = constrain(screenX, 0, width() - 1);
        screenY = constrain(screenY, 0, height() - 1);
        return true;
    }
    return false;
}
