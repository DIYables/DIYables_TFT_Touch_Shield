#ifndef DIYables_TFT_Touch_Shield_H
#define DIYables_TFT_Touch_Shield_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <DIYables_TFT_Shield.h>
#include "DIYables_TouchScreen.h"

// --- Touch Pin Mapping for RM68140 shield ---
#define TS_XP 6    // X+ (digital)
#define TS_YP A1   // Y+ (analog)
#define TS_XM A2   // X- (analog)
#define TS_YM 7    // Y- (digital)


class DIYables_TFT_RM68140_Shield : public DIYables_TFT_ILI9486_Shield {
public:
  /// Default: native display code + default touch pins (6, A1, A2, 7).
  DIYables_TFT_RM68140_Shield();

  /// Native display code + custom touch pins.
  /// Use this when the board is supported by the low-level driver but the
  /// touch panel is wired to different pins than the default Uno/Mega layout.
  DIYables_TFT_RM68140_Shield(uint8_t ts_xp, uint8_t ts_yp,
                               uint8_t ts_xm, uint8_t ts_ym);

  /// Arduino-API display code + custom touch pins.
  /// Use this when the board requires explicit pin assignments for both
  /// the 8-bit parallel data bus and the touch panel.
  DIYables_TFT_RM68140_Shield(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                               uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7,
                               uint8_t rd, uint8_t wr, uint8_t cd, uint8_t cs, uint8_t rst,
                               uint8_t ts_xp, uint8_t ts_yp, uint8_t ts_xm, uint8_t ts_ym);
  void begin();
  void invertDisplay(bool i);
  void setRotation(uint8_t r) override;
  void turnOn();
  void turnOff();
  void setTouchCalibration(int min_x, int max_x, int min_y, int max_y);
  void setADCResolution(uint8_t bits);
  void readTouchRaw(int &x, int &y, int &z);
  bool getTouch(int &screenX, int &screenY);

private:
  DIYables_TouchScreen _ts;
  uint8_t _ts_xp, _ts_yp, _ts_xm, _ts_ym;
  int touch_min_x = 136, touch_max_x = 907, touch_min_y = 942, touch_max_y = 139; // default for RM68140, override with setTouchCalibration
};

#endif // DIYables_TFT_Touch_Shield_H
