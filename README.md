## TFT LCD Touch Display Library for Arduino to work with DIYables TFT LCD Touch Screen Shield - DIYables_TFT_Touch_Shield
This Arduino library provides an easy-to-use interface for the [DIYables TFT LCD Touch Screen Shield](https://www.amazon.com/dp/B0DQ3NQ3LW).  
It enables fast and reliable graphics, text, and image rendering on your TFT shield.
* Fully extends the Adafruit GFX library, giving you access to all standard graphics and text functions.
* Adds optimized support for drawing shapes, displaying images, and rendering text specifically for the DIYables TFT LCD Display Shield.
* Compatible with a wide range of Arduino boards, including Uno R3, Uno R4 WiFi/Minima, Mega, Giga, Due, and more.

With this library, you can easily create colorful, high-resolution graphical interfaces for your Arduino projects using the DIYables TFT LCD Display Shield.

![DIYables TFT LCD Touch Display](https://newbiely.com/images/tutorial/tft-touch-screen-lcd-display-pinout.jpg)



Shield Version & Library Version
----------------------------
The DIYables TFT LCD Touch Display Shield has two hardware versions:

| Feature                | Older Version              | New Version                     |
|------------------------|----------------------------|---------------------------------|
| Driver IC              | ILI9488                    | RM68140                         |
| Resolution             | 320 × 480                  | 320 × 480                       |
| Touch Screen           | Resistive                  | Resistive                       |
| Interface              | 8-bit Parallel             | 8-bit Parallel                  |
| Screen Rotation        | ⚠️ Mirrored on some rotations | ✅ All rotations correct       |
| Drawing Speed          | Slow (pixel-by-pixel)      | ⚡ Hardware-accelerated          |
| Arduino Library        | v1.0.0                     | v2.0.0+                         |

* If you have the **older version (ILI9488)**, use library version **1.0.0**.
* If you have the **new version (RM68140)**, update to library version **2.0.0** or later via the Arduino Library Manager.
* Both versions use the same code structure and examples.



Product Link
----------------------------
* [TFT LCD Display](https://diyables.io/products/3.5-tft-lcd-color-touch-screen-shield-for-arduino-uno-mega-320x480-resolution-ili9488-driver-parallel-8-bit-interface-28pin-module-with-touch)
* [Amazon Link](https://www.amazon.com/dp/B0DQ3NQ3LW)



Features  
----------------------------  
* Works with all hardware platforms, including Arduino Uno R3, Arduino Uno R4 WiFi/Minima, Arduino Mega, Arduino Giga, Arduino Due...


Available Examples  
----------------------------  
* **DrawImage.ino**: Demonstrates how to display a bitmap image stored in program memory (flash) directly on the TFT screen.  
* **DrawImageSDcard.ino**: Shows how to load and display an image from an SD card onto the TFT display.  
* **DrawShapes.ino**: Illustrates drawing various geometric shapes (such as circles, rectangles, triangles, and more) on the TFT screen using the library’s graphics functions.  
* **ShowTextAndNumber.ino**: Demonstrates how to display text strings and numbers in different fonts, sizes, and colors on the TFT display.  
* **UseExternalFont.ino**: Shows how to use and display external (custom) fonts on the TFT screen for enhanced text appearance.  
* **TouchCalibration.ino**: Guides you through calibrating the touch screen for accurate touch detection and mapping.  
* **TouchGetPoint.ino**: Shows how to read and display the raw and mapped touch coordinates from the touch panel.
* **TouchButton.ino**: Demonstrates how to create a touchable button and detect press/release events on the TFT touch screen.  
* **TouchDraw.ino**: Lets you draw freehand lines on the screen by dragging your finger or stylus across the touch panel.  
* **TurnOnOff.ino**: Demonstrates how to turn the TFT display on and off using the `turnOn()` and `turnOff()` functions.  





Tutorials
----------------------------
* [Arduino - TFT LCD Touch Screen Display](https://arduinogetstarted.com/tutorials/arduino-tft-lcd-touch-screen-display)
* [Arduino UNO R4 - TFT LCD Touch Screen Display](https://newbiely.com/tutorials/arduino-uno-r4/arduino-uno-r4-tft-lcd-touch-screen-display)
* [Arduino Mega - TFT LCD Touch Screen Display](https://newbiely.com/tutorials/arduino-mega/arduino-mega-tft-lcd-touch-screen-display)



References
----------------------------
* [DIYables_TFT_Touch_Shield Library Reference](https://arduinogetstarted.com/reference/library/diyables-tft-touch-display-library)


Supported and Tested Hardware
----------------------------

| Board                   | Tested | Mode                      | Speed     |
|-------------------------|:------:|---------------------------|-----------|
| Arduino Uno R3          |   ✅   | Direct register access    | Very fast |
| [DIYables STEM V3 *(works like Arduino Uno R3)*](https://diyables.io/products/diyables-stem-v3-fully-compatible-with-arduino-uno-r3) |   ✅   | Direct register access    | Very fast |
| Arduino Uno R4 WiFi     |   ✅   | Direct register access    | Very fast |
| [DIYables STEM V4 IoT *(works like Arduino Uno R4 WiFi)*](https://diyables.io/products/diyables-stem-v4-iot-fully-compatible-with-arduino-uno-r4-wifi) |   ✅   | Direct register access    | Very fast |
| [DIYables STEM V4B IoT *(works like Arduino Uno R4 WiFi)*](https://diyables.io/products/diyables-stem-v4b-iot-development-board-compatible-with-arduino-uno-r4-wifi-ra4m1-32-bit-arm-cortex-m4-with-esp32-s3-wifi-bluetooth-usb-c-for-learning-prototyping-education) |   ✅   | Direct register access    | Very fast |
| Arduino Uno R4 Minima   |   ✅   | Direct register access    | Very fast |
| [DIYables STEM V4 Edu *(works like Arduino Uno R4 Minima)*](https://diyables.io/stem-v4-edu) |   ✅   | Direct register access    | Very fast |
| Arduino Mega            |   ✅   | Direct register access    | Very fast |
| [DIYables MEGA2560 R3 *(works like Arduino Mega 2560 Rev3)*](https://diyables.io/atmega2560-board) |   ✅   | Direct register access    | Very fast |
| Arduino Due             |   ✅   | Direct register access    | Very fast |
| Arduino Giga            |   ✅   | Direct register access    | Very fast |
| [DIYables ESP32 S3, Uno-form factor](https://diyables.io/products/esp32-s3-development-board-with-esp32-s3-wroom-1-n16r8-wifi-bluetooth-uno-compatible-form-factor-works-with-arduino-ide) |   ✅   | Direct register access    | Very fast |
| Other boards            |   —    | Arduino API (digitalWrite)| Slower    |





**Note**: This library is designed for the [DIYables 3.5 Inch TFT LCD Touch Display Shield](https://www.amazon.com/dp/B0DQ3NQ3LW) (with touch). It supports both the older ILI9488 version (library v1.0.0) and the new RM68140 version (library v2.0.0+). If you're using the [DIYables 3.5 Inch ILI9486 TFT LCD Display Screen Shield (non-touch)](https://www.amazon.com/dp/B0DLMV7NTK), please use the [DIYables TFT Shield Library](https://github.com/DIYables/DIYables_TFT_Shield) instead.

