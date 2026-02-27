#include <DIYables_TFT_Shield.h>
#include <DIYables_TFT_Touch_Shield.h>

DIYables_TFT_RM68140_Shield::DIYables_TFT_RM68140_Shield()
  : DIYables_TFT_ILI9486_Shield(), _ts(TS_XP, TS_YP, TS_XM, TS_YM, 300) {}

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

    _writeCmd(0x01);   // Software Reset — clears all registers to defaults
    delay(120);        // Wait for reset to complete

    _writeCmd(0x11);   // Sleep Out
    delay(120);

    _writeCmd(0x3A);   // Interface Pixel Format
    _writeData8(0x55); // 16-bit/pixel (RGB565)

    // Power Control
    _writeCmd(0xC0);
    _writeData8(0x0D); _writeData8(0x0D);
    _writeCmd(0xC1);
    _writeData8(0x43); _writeData8(0x00);
    _writeCmd(0xC2);
    _writeData8(0x00);

    // VCOM Control — affects colour balance and contrast
    _writeCmd(0xC5);
    _writeData8(0x00); _writeData8(0x48);
    _writeData8(0x00); _writeData8(0x48);

    // Display Function Control
    _writeCmd(0xB6);
    _writeData8(0x00);
    _writeData8(0x22); // SS=1, GS=0, REV=1
    _writeData8(0x3B); // NL = 480 lines

    // Positive Gamma Correction
    _writeCmd(0xE0);
    _writeData8(0x0F); _writeData8(0x21); _writeData8(0x1C); _writeData8(0x0B);
    _writeData8(0x0E); _writeData8(0x08); _writeData8(0x49); _writeData8(0x98);
    _writeData8(0x38); _writeData8(0x09); _writeData8(0x11); _writeData8(0x03);
    _writeData8(0x14); _writeData8(0x10); _writeData8(0x00);

    // Negative Gamma Correction
    _writeCmd(0xE1);
    _writeData8(0x0F); _writeData8(0x2F); _writeData8(0x2B); _writeData8(0x0C);
    _writeData8(0x0E); _writeData8(0x06); _writeData8(0x47); _writeData8(0x76);
    _writeData8(0x37); _writeData8(0x07); _writeData8(0x11); _writeData8(0x04);
    _writeData8(0x23); _writeData8(0x1E); _writeData8(0x00);

    _writeCmd(0x13);   // Normal Display Mode ON
    _writeCmd(0x29);   // Display ON
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
  _writeCmd(0x00); // NOP - ensure not in data mode
  _writeCmd(0x11); // Sleep Out
  delay(120);      // Wait for sleep out to complete
  _writeCmd(0x29); // Display ON
  delay(20);
}

// Turn off the display content. The screen goes white/blank, but the backlight
// remains on because it is hardwired to the power rail on the shield PCB and
// cannot be controlled by software. The frame buffer content is preserved, so
// calling turnOn() will restore the previous display content without redrawing.
void DIYables_TFT_RM68140_Shield::turnOff() {
  _writeCmd(0x00); // NOP - ensure not in data mode
  _writeCmd(0x28); // Display OFF
  delay(20);
  _writeCmd(0x10); // Sleep In (low power mode)
  delay(120);      // Wait for sleep in to complete
}


// --- Low-level write helpers (parent's writeCommand/writeData are private) ---

inline void DIYables_TFT_RM68140_Shield::_writeCmd(uint8_t cmd) {
  #ifndef ARDUINO_API_USED
  PIN_LOW(CD_PORT, CD_PIN);
  #else
  PIN_LOW(API_PIN_CD);
  #endif
  WRITE_8(cmd);
  #ifndef ARDUINO_API_USED
  PIN_LOW(WR_PORT, WR_PIN);
  asm volatile("nop");
  PIN_HIGH(WR_PORT, WR_PIN);
  #else
  PIN_LOW(API_PIN_WR);
  PIN_HIGH(API_PIN_WR);
  #endif
}

inline void DIYables_TFT_RM68140_Shield::_writeData8(uint8_t data) {
  #ifndef ARDUINO_API_USED
  PIN_HIGH(CD_PORT, CD_PIN);
  #else
  PIN_HIGH(API_PIN_CD);
  #endif
  WRITE_8(data);
  #ifndef ARDUINO_API_USED
  PIN_LOW(WR_PORT, WR_PIN);
  asm volatile("nop");
  PIN_HIGH(WR_PORT, WR_PIN);
  #else
  PIN_LOW(API_PIN_WR);
  PIN_HIGH(API_PIN_WR);
  #endif
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
  _writeCmd(0xB6);
  _writeData8(0x00);
  _writeData8(GS | SS | 0x02);  // GS, SS, REV=1
  _writeData8(0x3B);            // NL = 480 lines

  // MADCTL (0x36): only MV and BGR (no MX/MY)
  _writeCmd(0x36);
  _writeData8(val);
}


// --- Hardware-accelerated drawing ---
// The parent class has setAddrWindow/writeData16 as private, so we replicate
// them here to enable bulk-pixel fills for rectangles and lines.

inline void DIYables_TFT_RM68140_Shield::_pulseWR() {
  #ifndef ARDUINO_API_USED
  PIN_LOW(WR_PORT, WR_PIN);
  asm volatile("nop");
  PIN_HIGH(WR_PORT, WR_PIN);
  #else
  PIN_LOW(API_PIN_WR);
  PIN_HIGH(API_PIN_WR);
  #endif
}

inline void DIYables_TFT_RM68140_Shield::_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  _writeCmd(0x2A);
  _writeData8(x0 >> 8); _writeData8(x0 & 0xFF);
  _writeData8(x1 >> 8); _writeData8(x1 & 0xFF);

  _writeCmd(0x2B);
  _writeData8(y0 >> 8); _writeData8(y0 & 0xFF);
  _writeData8(y1 >> 8); _writeData8(y1 & 0xFF);

  _writeCmd(0x2C);
}

void DIYables_TFT_RM68140_Shield::_writeData16(uint16_t data, uint32_t count) {
  uint8_t hi = data >> 8;
  uint8_t lo = data & 0xFF;

  #ifndef ARDUINO_API_USED
  PIN_HIGH(CD_PORT, CD_PIN);
  #else
  PIN_HIGH(API_PIN_CD);
  #endif

  while (count >= 8) {
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    WRITE_8(hi); _pulseWR(); WRITE_8(lo); _pulseWR();
    count -= 8;
  }
  while (count--) {
    WRITE_8(hi); _pulseWR();
    WRITE_8(lo); _pulseWR();
  }
}

void DIYables_TFT_RM68140_Shield::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  // Bounds clipping
  if (x >= width() || y >= height() || w <= 0 || h <= 0) return;
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if ((x + w) > width())  w = width()  - x;
  if ((y + h) > height()) h = height() - y;

  _setAddrWindow(x, y, x + w - 1, y + h - 1);
  _writeData16(color, (uint32_t)w * h);
}

void DIYables_TFT_RM68140_Shield::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  fillRect(x, y, w, 1, color);
}

void DIYables_TFT_RM68140_Shield::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  fillRect(x, y, 1, h, color);
}

void DIYables_TFT_RM68140_Shield::fillScreen(uint16_t color) {
  _setAddrWindow(0, 0, width() - 1, height() - 1);
  _writeData16(color, (uint32_t)width() * height());
}

// Public address window for advanced users (e.g., streaming pixels from SD card)
void DIYables_TFT_RM68140_Shield::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  _setAddrWindow(x0, y0, x1, y1);
}

// Stream an array of 16-bit pixel colors (from RAM) to the display.
// Call setAddrWindow() first to define the target region.
void DIYables_TFT_RM68140_Shield::pushColors(uint16_t *data, uint32_t len) {
  #ifndef ARDUINO_API_USED
  PIN_HIGH(CD_PORT, CD_PIN);
  #else
  PIN_HIGH(API_PIN_CD);
  #endif

  while (len--) {
    uint16_t color = *data++;
    WRITE_8(color >> 8); _pulseWR();
    WRITE_8(color & 0xFF); _pulseWR();
  }
}

// Hardware-accelerated PROGMEM bitmap drawing.
// Sets the address window once and streams all pixels.
void DIYables_TFT_RM68140_Shield::drawRGBBitmap(int16_t x, int16_t y,
  const uint16_t bitmap[], int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  // Fully on screen: fast path
  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    _setAddrWindow(x, y, x + w - 1, y + h - 1);

    #ifndef ARDUINO_API_USED
    PIN_HIGH(CD_PORT, CD_PIN);
    #else
    PIN_HIGH(API_PIN_CD);
    #endif

    uint32_t count = (uint32_t)w * h;
    for (uint32_t i = 0; i < count; i++) {
      uint16_t color = pgm_read_word(&bitmap[i]);
      WRITE_8(color >> 8); _pulseWR();
      WRITE_8(color & 0xFF); _pulseWR();
    }
  } else {
    // Partially off screen: clip per pixel
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, pgm_read_word(&bitmap[j * w + i]));
        }
      }
    }
  }
}

// Hardware-accelerated RAM bitmap drawing.
void DIYables_TFT_RM68140_Shield::drawRGBBitmap(int16_t x, int16_t y,
  uint16_t *bitmap, int16_t w, int16_t h) {
  if (x >= width() || y >= height() || (x + w) <= 0 || (y + h) <= 0) return;

  if (x >= 0 && y >= 0 && (x + w) <= width() && (y + h) <= height()) {
    _setAddrWindow(x, y, x + w - 1, y + h - 1);
    pushColors(bitmap, (uint32_t)w * h);
  } else {
    for (int16_t j = 0; j < h; j++) {
      for (int16_t i = 0; i < w; i++) {
        int16_t px = x + i, py = y + j;
        if (px >= 0 && px < width() && py >= 0 && py < height()) {
          drawPixel(px, py, bitmap[j * w + i]);
        }
      }
    }
  }
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
    // TouchScreen.getPoint() leaves TS_YP and TS_XM in INPUT (analog) mode,
    // but these pins are shared with the TFT data/control bus.
    pinMode(TS_YP, OUTPUT);
    pinMode(TS_XM, OUTPUT);

    // Restore all TFT data bus and control pin directions
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
                screenX = map(raw_x, touch_min_x, touch_max_x, 0, width() - 1);
                screenY = map(raw_y, touch_min_y, touch_max_y, 0, height() - 1);
                break;
            case 1:
                screenX = map(raw_y, touch_min_y, touch_max_y, 0, width() - 1);
                screenY = map(raw_x, touch_max_x, touch_min_x, 0, height() - 1);
                break;
            case 2:
                screenX = map(raw_x, touch_max_x, touch_min_x, 0, width() - 1);
                screenY = map(raw_y, touch_max_y, touch_min_y, 0, height() - 1);
                break;
            case 3:
                screenX = map(raw_y, touch_max_y, touch_min_y, 0, width() - 1);
                screenY = map(raw_x, touch_min_x, touch_max_x, 0, height() - 1);
                break;
            default:
                screenX = map(raw_x, touch_min_x, touch_max_x, 0, width() - 1);
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
