#ifndef DIYables_TFT_Touch_Shield_H
#define DIYables_TFT_Touch_Shield_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <DIYables_TFT_Shield.h>
#include <TouchScreen.h>

// --- Touch Pin Mapping for RM68140 shield ---
#define TS_XP 6    // X+ (digital)
#define TS_YP A1   // Y+ (analog)
#define TS_XM A2   // X- (analog)
#define TS_YM 7    // Y- (digital)


class DIYables_TFT_RM68140_Shield : public DIYables_TFT_ILI9486_Shield {
public:
  DIYables_TFT_RM68140_Shield();
  void begin();
  void invertDisplay(bool i);
  void setRotation(uint8_t r) override;
  void fillScreen(uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
  void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);
  void drawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h);
  void pushColors(uint16_t *data, uint32_t len);
  void setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
  void turnOn();
  void turnOff();
  void setTouchCalibration(int min_x, int max_x, int min_y, int max_y);
  void readTouchRaw(int &x, int &y, int &z);
  bool getTouch(int &screenX, int &screenY);

private:
  TouchScreen _ts;
  int touch_min_x = 136, touch_max_x = 907, touch_min_y = 942, touch_max_y = 139; // default for RM68140, override with setTouchCalibration
  inline void _writeCmd(uint8_t cmd);
  inline void _writeData8(uint8_t data);
  inline void _pulseWR();
  inline void _setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void _writeData16(uint16_t data, uint32_t count);
};

#endif // DIYables_TFT_Touch_Shield_H
