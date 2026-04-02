// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
//
// Originally written by ladyada / Adafruit Industries
// (c) 2019 Limor Fried for Adafruit Industries
// Original source: https://github.com/adafruit/Adafruit_TouchScreen
// Code under BSD License — see DIYables_TouchScreen.h for full notice.
//
// Copied and modified by DIYables for use in DIYables_TFT_Touch_Shield.
// Key modification: DIYABLES_TS_ADC_MAX replaces all hardcoded 1023/1024
// values so that 12-bit ADC platforms (Due, Giga, SAMD, STM32, RP2040)
// return correct coordinates and pressure readings without any sketch-level
// analogReadResolution() calls.

#include "Arduino.h"
#include "pins_arduino.h"
#include "DIYables_TouchScreen.h"

// ---------------------------------------------------------------------------
// MBED analogRead() cache workaround
//
// The MBED Arduino core (used by Arduino Giga R1 WiFi, Portenta H7, Nano 33
// BLE, etc.) caches the mbed::AnalogIn object created by the first
// analogRead() on each pin.  Subsequent analogRead() calls reuse the cached
// object without reconfiguring the pin.  When pinMode(OUTPUT/INPUT) is called
// between analogRead() calls (as the touch driver must do to share pins with
// the TFT data bus), the GPIO MODER register is changed from analog (0b11)
// to digital (0b00 or 0b01), but the cached AnalogIn never switches it back.
// Result: analogRead() returns a stale constant regardless of actual voltage.
//
// Fix: delete the cached object before analogRead() so a fresh AnalogIn is
// created, which re-runs analogin_init() and sets MODER back to analog.
// ---------------------------------------------------------------------------
#ifdef ARDUINO_ARCH_MBED
  #include "pinDefinitions.h"
  static void resetAnalogPinCache(pin_size_t pin) {
    mbed::AnalogIn*& adc = analogPinToAdcObj(pin);
    if (adc != NULL) {
      delete adc;
      adc = NULL;
    }
  }
#endif

// Oversampling count.
// 3 = take 3 samples and return the median (recommended — robust against
//     single noisy reads and works correctly on all platforms including
//     Arduino Giga / STM32H7 whose ADC has higher inter-sample variation
//     than AVR when reading through the resistive touch panel).
// 2 = dual sample; only accepted when BOTH readings agree within
//     DIYABLES_TS_NOISE counts.  Too strict for Giga → rejects every read.
// 1 = no oversampling (immediate read)
#define NUMSAMPLES 3

// ---------------------------------------------------------------------------
// TSPoint
// ---------------------------------------------------------------------------

TSPoint::TSPoint(void) { x = y = z = 0; }

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool TSPoint::operator==(TSPoint p1) {
  return ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TSPoint::operator!=(TSPoint p1) {
  return ((p1.x != x) || (p1.y != y) || (p1.z != z));
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

#if (NUMSAMPLES > 2)
static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;
  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save;
  }
}
#endif

// ---------------------------------------------------------------------------
// DIYables_TouchScreen
// ---------------------------------------------------------------------------

DIYables_TouchScreen::DIYables_TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm,
                                           uint8_t ym, uint16_t rxplate)
    : _xp(xp), _yp(yp), _xm(xm), _ym(ym), _rxplate(rxplate) {
  pressureThreshhold = 10;
  setADCResolution(DIYABLES_TS_ADC_DEFAULT_BITS);
}

void DIYables_TouchScreen::setADCResolution(uint8_t bits) {
  _adc_div   = 1 << bits;        // e.g. 1024 for 10-bit
  _adc_max   = _adc_div - 1;     // e.g. 1023
  _adc_noise = _adc_max / 256;   // e.g. 3 for 10-bit, 15 for 12-bit
}

// Read the X axis (drives X+/X- and samples Y+).
int DIYables_TouchScreen::readTouchX(void) {
  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);
  digitalWrite(_yp, LOW);
  digitalWrite(_ym, LOW);

  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, HIGH);
  pinMode(_xm, OUTPUT);
  digitalWrite(_xm, LOW);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_yp);
  delayMicroseconds(200);
#endif

  return (_adc_max - analogRead(_yp));
}

// Read the Y axis (drives Y+/Y- and samples X-).
int DIYables_TouchScreen::readTouchY(void) {
  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  digitalWrite(_xp, LOW);
  digitalWrite(_xm, LOW);

  pinMode(_yp, OUTPUT);
  digitalWrite(_yp, HIGH);
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, LOW);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_xm);
  delayMicroseconds(200);
#endif

  return (_adc_max - analogRead(_xm));
}

// Read Z/pressure.
uint16_t DIYables_TouchScreen::pressure(void) {
  // X+ → GND, Y- → VCC, hi-Z X- and Y+
  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, LOW);
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, HIGH);

  digitalWrite(_xm, LOW);
  pinMode(_xm, INPUT);
  digitalWrite(_yp, LOW);
  pinMode(_yp, INPUT);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_xm);
  resetAnalogPinCache(_yp);
  delayMicroseconds(200);
#endif

  int z1 = analogRead(_xm);
  int z2 = analogRead(_yp);

  if (_rxplate != 0) {
    float rtouch;
    rtouch  = z2;
    rtouch /= z1;
    rtouch -= 1;
    rtouch *= readTouchX();
    rtouch *= _rxplate;
    rtouch /= _adc_div;
    return (uint16_t)rtouch;
  } else {
    return (_adc_max - (z2 - z1));
  }
}

// Read X, Y, and Z together with oversampling/noise filtering.
TSPoint DIYables_TouchScreen::getPoint(void) {
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i;
  uint8_t valid = 1;

  // --- Read X ---
  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);
  pinMode(_xp, OUTPUT);
  pinMode(_xm, OUTPUT);
  digitalWrite(_xp, HIGH);
  digitalWrite(_xm, LOW);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_yp);
  delayMicroseconds(200);
#endif

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(_yp);
  }

#if NUMSAMPLES > 2
  insert_sort(samples, NUMSAMPLES);
#endif

#if NUMSAMPLES == 2
  // Reject if difference exceeds noise tolerance (scaled to ADC resolution)
  if (samples[0] - samples[1] < -_adc_noise ||
      samples[0] - samples[1] >  _adc_noise) {
    valid = 0;
  } else {
    samples[1] = (samples[0] + samples[1]) >> 1; // average
  }
#endif

  x = (_adc_max - samples[NUMSAMPLES / 2]);

  // --- Read Y ---
  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  pinMode(_yp, OUTPUT);
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, LOW);
  digitalWrite(_yp, HIGH);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_xm);
  delayMicroseconds(200);
#endif

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(_xm);
  }

#if NUMSAMPLES > 2
  insert_sort(samples, NUMSAMPLES);
#endif

#if NUMSAMPLES == 2
  if (samples[0] - samples[1] < -_adc_noise ||
      samples[0] - samples[1] >  _adc_noise) {
    valid = 0;
  } else {
    samples[1] = (samples[0] + samples[1]) >> 1;
  }
#endif

  y = (_adc_max - samples[NUMSAMPLES / 2]);

  // --- Read Z/pressure ---
  // X+ → GND, Y- → VCC, hi-Z X- and Y+
  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, LOW);
  digitalWrite(_yp, LOW);
  pinMode(_yp, INPUT);
  digitalWrite(_ym, HIGH);

#ifdef ARDUINO_ARCH_MBED
  resetAnalogPinCache(_xm);
  resetAnalogPinCache(_yp);
  delayMicroseconds(200);
#endif

  int z1 = analogRead(_xm);
  int z2 = analogRead(_yp);

  if (_rxplate != 0) {
    // Guard against division by zero: z1=0 means no touch (no conductive path).
    // Without this guard, (float)z2/0 → +inf on ARM HW-FPU, and the subsequent
    // (int) cast is undefined behaviour — on STM32H7 it produces INT_MIN/MAX,
    // which the caller cannot distinguish from a valid large pressure value.
    if (z1 == 0) {
      z = 0;
    } else {
      float rtouch = (float)z2 / z1 - 1.0f;
      rtouch *= x;
      rtouch *= _rxplate;
      rtouch /= _adc_div;
      z = (rtouch > 0.0f) ? (int)rtouch : 0;
    }
  } else {
    z = (_adc_max - (z2 - z1));
  }

  if (!valid) {
    z = 0;
  }

  return TSPoint(x, y, z);
}

bool DIYables_TouchScreen::isTouching(void) {
  return (pressure() > pressureThreshhold);
}
