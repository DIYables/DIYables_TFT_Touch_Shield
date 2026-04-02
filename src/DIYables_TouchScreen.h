// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
//
// Originally written by ladyada / Adafruit Industries
// (c) 2019 Limor Fried for Adafruit Industries
// Original source: https://github.com/adafruit/Adafruit_TouchScreen
// Code under BSD License (see license note below)
//
// Copied and modified by DIYables for use in DIYables_TFT_Touch_Shield.
// Modifications:
//   - Removed USE_FAST_PINIO path (not needed for target platforms)
//   - ADC resolution is runtime-configurable via setADCResolution(bits).
//     The default is auto-detected at compile time (10-bit for most platforms,
//     12-bit for RP2040).  Call setADCResolution() to override when the
//     platform uses a non-default resolution or after analogReadResolution().
//
// BSD License
// -----------
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holders nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.

#ifndef DIYABLES_TOUCHSCREEN_H
#define DIYABLES_TOUCHSCREEN_H

#include <stdint.h>

// ---------------------------------------------------------------------------
// Default ADC resolution (used to initialise instance variables).
// Most Arduino-API platforms keep analogRead() at 10-bit (0-1023) by default.
// Only the RP2040 (Earle Philhower core) genuinely defaults to 12-bit.
// Users can override at runtime with setADCResolution(bits).
// ---------------------------------------------------------------------------
#if defined(ARDUINO_ARCH_RP2040)
  #define DIYABLES_TS_ADC_DEFAULT_BITS  12
#else
  #define DIYABLES_TS_ADC_DEFAULT_BITS  10
#endif

// ---------------------------------------------------------------------------

/** Encapsulates a single touch measurement (X, Y, Z/pressure). */
class TSPoint {
public:
  TSPoint(void);
  TSPoint(int16_t x, int16_t y, int16_t z);

  bool operator==(TSPoint);
  bool operator!=(TSPoint);

  int16_t x, y, z;
};

/** Controls and keeps state for a resistive touch screen. */
class DIYables_TouchScreen {
public:
  DIYables_TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym,
                       uint16_t rx);

  /** Set the ADC resolution in bits (e.g. 10, 12, 14, 16).
   *  Call this if you use analogReadResolution() in your sketch or if
   *  the platform default is not 10-bit. */
  void     setADCResolution(uint8_t bits);

  bool     isTouching(void);
  uint16_t pressure(void);
  int      readTouchY(void);
  int      readTouchX(void);
  TSPoint  getPoint(void);

  int16_t pressureThreshhold; ///< Pressure threshold for isTouching()

private:
  // Touch panel pin assignments (Arduino pin numbers, e.g. 6, A1, A2, 7).
  // XP/XM drive the X-axis; YP/YM drive the Y-axis.
  // The two analog-capable pins (typically YP and XM) are also used for
  // analogRead() to sense touch position and pressure.
  uint8_t  _yp, _ym, _xm, _xp;
  uint16_t _rxplate;
  int      _adc_max;  ///< (1 << bits) - 1, e.g. 1023 for 10-bit
  int      _adc_div;  ///< (1 << bits),     e.g. 1024 for 10-bit
  int      _adc_noise; ///< _adc_max / 256
};

#endif // DIYABLES_TOUCHSCREEN_H
