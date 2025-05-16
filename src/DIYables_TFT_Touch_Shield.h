#ifndef DIYables_TFT_Touch_Shield_H
#define DIYables_TFT_Touch_Shield_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <DIYables_TFT_Shield.h>

// --- Touch Pin Mapping for your shield ---
#define YP A2  // Y+ (analog)
#define XM A3  // X- (analog)
#define YM 8   // Y- (digital)
#define XP 9   // X+ (digital)


class DIYables_TFT_ILI9488_Shield : public DIYables_TFT_ILI9486_Shield {
public:
  DIYables_TFT_ILI9488_Shield();
  void begin();
  void invertDisplay(bool i);
  void setTouchCalibration(int min_x, int max_x, int min_y, int max_y);
  void readTouchRaw(int &x, int &y, int &z);
  bool getTouch(int &screenX, int &screenY);

private:
  int touch_min_x = 121, touch_max_x = 913, touch_min_y = 78, touch_max_y = 931;// default, override with setTouchCalibration
};


#endif // DIYables_TFT_Touch_Shield_H