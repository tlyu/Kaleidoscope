/* -*- mode: c++ -*-
 * Kaleidoscope-Hardware-Adafruit-NeoTrellis -- Adafruit NeoTrellis hardware support for Kaleidoscope
 * Copyright (C) 2017-2021  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#if USE_TINYUSB

#include <Arduino.h>

#if KALEIDOSCOPE_USE_NEOTRELLIS
#include <Adafruit_NeoTrellis.h>
#endif

#define CRGB(r, g, b) \
  (cRGB) {            \
    r, g, b           \
  }

struct cRGB {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

#define _INIT_HID_GETSHORTNAME

#include "kaleidoscope/device/Base.h"
#include "kaleidoscope/driver/hid/Base.h"
#include "kaleidoscope/driver/hid/TinyUSB.h"
#include "kaleidoscope/driver/keyscanner/Base.h"
#include "kaleidoscope/driver/led/Base.h"
#include "kaleidoscope/driver/mcu/TinyUSB.h"


namespace kaleidoscope {
namespace device {
namespace adafruit {

struct NeoTrellisLEDDriverProps : public kaleidoscope::driver::led::BaseProps {
  static constexpr uint8_t led_count             = 16;
  static constexpr uint8_t key_led_map[] PROGMEM = {
    // clang-format off
     0,  1,  2,  3,
     4,  5,  6,  7,
     8,  9, 10, 11,
    12, 13, 14, 15,
    // clang-format on
  };
};

class NeoTrellisLEDDriver : public kaleidoscope::driver::led::Base<NeoTrellisLEDDriverProps> {
 public:
#if KALEIDOSCOPE_USE_NEOTRELLIS
  static void syncLeds();
  static void setCrgbAt(uint8_t i, cRGB crgb);
  static cRGB getCrgbAt(uint8_t i);
#endif
};


struct NeoTrellisKeyScannerProps : public kaleidoscope::driver::keyscanner::BaseProps {
  static constexpr uint8_t matrix_rows    = 4;
  static constexpr uint8_t matrix_columns = 4;
  typedef MatrixAddr<matrix_rows, matrix_columns> KeyAddr;
};

class NeoTrellisKeyScanner : public kaleidoscope::driver::keyscanner::Base<NeoTrellisKeyScannerProps> {
#if KALEIDOSCOPE_USE_NEOTRELLIS
 private:
  typedef NeoTrellisKeyScanner ThisType;
  static uint16_t last_scan;
  static bool inited;

 public:
  static void setup();
  static void scanMatrix();
  static void readMatrix();

 protected:
#endif
};


struct NeoTrellisProps : public kaleidoscope::device::BaseProps {
  typedef kaleidoscope::driver::hid::TinyUSBProps HIDProps;
  typedef kaleidoscope::driver::hid::TinyUSB<HIDProps> HID;

  typedef NeoTrellisLEDDriverProps LEDDriverProps;
  typedef NeoTrellisLEDDriver LEDDriver;

  typedef NeoTrellisKeyScannerProps KeyScannerProps;
  typedef NeoTrellisKeyScanner KeyScanner;

  typedef kaleidoscope::driver::mcu::TinyUSBProps MCUProps;
  typedef kaleidoscope::driver::mcu::TinyUSB<MCUProps> MCU;

  static constexpr const char *short_name = "neotrellis";
};

class NeoTrellis : public kaleidoscope::device::Base<NeoTrellisProps> {
 public:
  auto serialPort() -> decltype(Serial) & {
    return Serial;
  }
#if KALEIDOSCOPE_USE_NEOTRELLIS
  void setup() {
    trellis.begin();
    Base<NeoTrellisProps>::setup();
  }
#endif

 private:
#if KALEIDOSCOPE_USE_NEOTRELLIS
  friend class NeoTrellisLEDDriver;
  friend class NeoTrellisKeyScanner;
  static Adafruit_NeoTrellis trellis;
#endif
};


}  // namespace adafruit
}  // namespace device

EXPORT_DEVICE(kaleidoscope::device::adafruit::NeoTrellis)

}  // namespace kaleidoscope

// clang-format off

#define PER_KEY_DATA_STACKED(dflt, \
               r0c0, r0c1, r0c2, r0c3, \
               r1c0, r1c1, r1c2, r1c3, \
               r2c0, r2c1, r2c2, r2c3, \
               r3c0, r3c1, r3c2, r3c3, \
               ...) \
    r0c0, r0c1, r0c2, r0c3, \
    r1c0, r1c1, r1c2, r1c3, \
    r2c0, r2c1, r2c2, r2c3, \
    r3c0, r3c1, r3c2, \
    RESTRICT_ARGS_COUNT((r3c3), 16, KEYMAP_STACKED, ##__VA_ARGS__)

#define PER_KEY_DATA(dflt, \
               r0c0, r0c1, r0c2, r0c3, \
               r1c0, r1c1, r1c2, r1c3, \
               r2c0, r2c1, r2c2, r2c3, \
               r3c0, r3c1, r3c2, r3c3, \
               ...) \
    r0c0, r0c1, r0c2, r0c3, \
    r1c0, r1c1, r1c2, r1c3, \
    r2c0, r2c1, r2c2, r2c3, \
    r3c0, r3c1, r3c2, \
    RESTRICT_ARGS_COUNT((r3c3), 16, KEYMAP, ##__VA_ARGS__)

#endif
