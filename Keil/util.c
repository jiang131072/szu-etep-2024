#include "util.h"

const uint8_t digit2seg[] = {0x03, 0x9f, 0x25, 0x0d, 0x99,
                             0x49, 0x41, 0x1f, 0x01, 0x09};

void delay_ms(uint16_t n) {
  uint16_t i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < 123; j++)
      ;
  }
}