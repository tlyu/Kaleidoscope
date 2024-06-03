/* -*- mode: c++ -*-
 * Kaleidoscope-Hardware-Adafruit-NeoTrellis -- Adafruit NeoTrellis hardware support for Kaleidoscope
 * Copyright (C) 2021  Keyboard.io, Inc
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

#if USE_TINYUSB && KALEIDOSCOPE_USE_NEOTRELLIS

#include "kaleidoscope/device/adafruit/NeoTrellis.h"

#include <Arduino.h>  // for PROGMEM
#include <Adafruit_NeoTrellis.h>
#include "kaleidoscope/driver/keyscanner/Base_Impl.h"  // For Base<>

namespace kaleidoscope {
namespace device {
namespace adafruit {

Adafruit_NeoTrellis NeoTrellis::trellis;

constexpr uint8_t NeoTrellisLEDDriverProps::key_led_map[] PROGMEM;

void NeoTrellisLEDDriver::syncLeds() {
  NeoTrellis::trellis.pixels.show();
}

void NeoTrellisLEDDriver::setCrgbAt(uint8_t i, cRGB crgb) {
  NeoTrellis::trellis.pixels.setPixelColor(i, crgb.r, crgb.g, crgb.b);
}

cRGB NeoTrellisLEDDriver::getCrgbAt(uint8_t i) {
  uint32_t raw = NeoTrellis::trellis.pixels.getPixelColor(i);
  return cRGB{(uint8_t)(raw >> 16), (uint8_t)(raw >> 8), (uint8_t)(raw)};
}

/********* Key scanner *********/

uint16_t NeoTrellisKeyScanner::last_scan = 0;
bool NeoTrellisKeyScanner::inited        = false;

static TrellisCallback cb(keyEvent evt) {
  uint8_t keystate = 0, row = 0, col = 0;
  if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    keystate = 0x02;
  } else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    keystate = 0x01;
  }
  col = evt.bit.NUM % 4;
  row = evt.bit.NUM / 4;
  if (keystate) {
    NeoTrellisKeyScanner::handleKeyswitchEvent(Key_NoKey, KeyAddr(row, col), keystate);
  }
  return 0;
}

void NeoTrellisKeyScanner::setup() {
  auto &trellis = NeoTrellis::trellis;
  if (inited) {
    return;
  }
  for (int i = 0; i < 16; i++) {
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
    trellis.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
    trellis.registerCallback(i, cb);
  }
  inited = true;
}

void NeoTrellisKeyScanner::readMatrix() {
  NeoTrellis::trellis.read();
}

void NeoTrellisKeyScanner::scanMatrix() {
  if (!inited) {
    return;
  }
  if (!Runtime.hasTimeExpired(last_scan, 20)) {
    return;
  }
  last_scan = millis();
  readMatrix();
}

}  // namespace adafruit
}  // namespace device
}  // namespace kaleidoscope

#endif
