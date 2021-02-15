/*
MIT License

Copyright (c) 2017 Tom Magnier
Modified 2018 by Nicolò Veronese
Adapted to DS2430A in 2021 by René van den Berg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include "DS2430A.h"

DS2430A::DS2430A(OneWire &ow) : _ow(ow) {
  _skiprom = true;
}

void DS2430A::begin(uint8_t serialNumber[ONE_WIRE_MAC_SIZE]) {
  memcpy(_serialNumber, serialNumber, ONE_WIRE_MAC_SIZE);
  _skiprom = false;
}

uint8_t DS2430A::read(uint8_t address) {
  uint8_t res = 0xFF;
  read(address, &res, 1);

  return res;
}

void DS2430A::read(uint8_t address, uint8_t *buf, uint8_t len) {
  _start_transmission();

  _ow.write(READ_MEMORY, 1);
  _ow.write(address, 1);

  for (int i = 0; i < len; i++) {
    buf[i] = _ow.read();
  }

  _ow.depower();
}

bool DS2430A::write(uint8_t address, const uint8_t *buf, uint8_t count, bool verify /* = 0 */) {
  bool ret = _write(address, buf, count, verify);
  _ow.depower();
  return ret;
}

bool DS2430A::_write(uint8_t address, const uint8_t *buf, uint8_t count, bool verify) {
  uint8_t scratchpad[DS2430A_EEPROM_SIZE];
  uint8_t command[DS2430A_CMD_SIZE];

  // Pre-verification: address has to be aligned on an 8-byte boundary
  if (address + count > DS2430A_EEPROM_SIZE) {
    // attempt to write outside of buffer
    return false;
  }

  // step 1: copy whole memory into buffer
  read(0x00, scratchpad, DS2430A_EEPROM_SIZE);

  // step 2: make modifications
  memcpy(&scratchpad[address], buf, count);

  // step 3: copy buffer to scratchpad
  command[0] = WRITE_SCRATCHPAD;
  command[1] = 0x00;
  _start_transmission();
  _ow.write_bytes(command, DS2430A_CMD_SIZE, 1);    // Write command + address
  _ow.write_bytes(scratchpad, DS2430A_EEPROM_SIZE, 1);  // Write scratchpad contents

  if (verify) {
    // TODO: insert verification here
    // Copy the scratchpad to some other buffer and use memcmp
    // to verify the contents are still the same
  }

  // step 4: copy scratchpad to rom
  command[0] = COPY_SCRATCHPAD;
  command[1] = DS2430A_COPY_SCRATCHPAD_AUTHORIZATION_CODE;   // Authorization code
  _start_transmission();
  _ow.write_bytes(command, DS2430A_CMD_SIZE, 1);

  delay(11);                                                  // t_prog = 10ms, wait a bit longer to be sure

  return true;
}
