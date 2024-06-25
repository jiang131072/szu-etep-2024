#include "util.h"
#include <STC89C5xRC.H>

#include "oled.h"
#include "oledfont.h"

void iic_start() {
  oled_scl_set();
  oled_sda_set();
  oled_sda_clr();
  oled_scl_clr();
}

void iic_stop() {
  oled_scl_set();
  oled_sda_clr();
  oled_sda_set();
}

void iic_wait_ack() {
  oled_scl_set();
  oled_scl_clr();
}

void iic_send_byte(uint8_t byte) {
  uint8_t i, m;

  oled_scl_clr();
  for (i = 0; i < 8; i++) {
    m = byte & 0x80;
    if (m == 0x80) {
      oled_sda_set();
    } else
      oled_sda_clr();
    byte <<= 1;
    oled_scl_set();
    oled_scl_clr();
  }
}

void iic_write_command(uint8_t byte) {
  iic_start();
  iic_send_byte(0x78); // Slave address, SA0=0
  iic_wait_ack();
  iic_send_byte(0x00); // write command
  iic_wait_ack();
  iic_send_byte(byte);
  iic_wait_ack();
  iic_stop();
}

void iic_write_data(uint8_t byte) {
  iic_start();
  iic_send_byte(0x78); // D/C#=0, R/W#=0
  iic_wait_ack();
  iic_send_byte(0x40); // write data
  iic_wait_ack();
  iic_send_byte(byte);
  iic_wait_ack();
  iic_stop();
}

void oled_write(uint8_t byte, bit type) {
  if (type == OLED_DATA) {
    iic_write_data(byte);
  } else { // is_data == OLED_CMD
    iic_write_command(byte);
  }
}

void oled_fill(uint8_t byte) {
  uint8_t m, n;
  for (m = 0; m < 8; m++) {
    oled_write(0xb0 + m, OLED_CMD);
    oled_write(0x00, OLED_CMD);
    oled_write(0x10, OLED_CMD);
    for (n = 0; n < 128; n++) {
      oled_write(byte, OLED_DATA);
    }
  }
}
void oled_clear(void) { oled_fill(0); }

void oled_show_colomn(uint8_t page, uint8_t colomn, uint8_t byte) {
  oled_write(0xb0 + page, OLED_CMD);
  oled_write(((colomn & 0xf0) >> 4) | 0x10, OLED_CMD);
  oled_write((colomn & 0x0f), OLED_CMD);
  oled_write(byte, OLED_DATA);
}

void oled_goto(uint8_t x, uint8_t y) {
  oled_write(0xb0 + y, OLED_CMD);
  oled_write(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
  oled_write((x & 0x0f), OLED_CMD);
}

void oled_show_ascii(uint8_t x, uint8_t y, uint8_t character) {
  uint8_t offset = character - ' ';
  uint8_t i = 0;

  if (x > MAX_COL - 1) {
    x = 0;
    y = y + 2;
  }

  oled_goto(x, y);
  for (i = 0; i < 6; i++)
    oled_write(ASCII_BITMAP[offset][i], OLED_DATA);
}
void oled_show_ascii_string(uint8_t x, uint8_t y, uint8_t *string) {
  unsigned char i = 0;
  while (string[i] != '\0') {
    oled_show_ascii(x, y, string[i]);
    x += 8;
    if (x > 120) {
      x = 0;
      y += 2;
    }
    i++;
  }
}

// pow for uint8_t, return uint32_t
// <math.h> only supports float
uint32_t pow_uint8_t(uint8_t m, uint8_t n) {
  uint32_t result = 1;
  while (n--)
    result *= m;
  return result;
}

void oled_show_uint32_t(uint8_t x, uint8_t y, uint32_t num, uint8_t len) {
  uint8_t t, temp, show = 0;
  for (t = 0; t < len; t++) {
    temp = (num / pow_uint8_t(10, len - t - 1)) % 10;
    if (show == 0 && t < (len - 1)) {
      if (temp == 0) {
        oled_show_ascii(x + 6 * t, y, ' ');
        continue;
      } else
        show = 1;
    }
    oled_show_ascii(x + 6 * t, y, temp + '0');
  }
}

void oled_init(void) {
  oled_write(0xAE, OLED_CMD); // display off
  oled_write(0x00, OLED_CMD); // set low column address
  oled_write(0x10, OLED_CMD); // set high column address
  oled_write(0x40, OLED_CMD); // set start line address
  oled_write(0xB0, OLED_CMD); // set page address
  oled_write(0x81, OLED_CMD); // contract control
  oled_write(0xFF, OLED_CMD); // 128
  oled_write(0xA1, OLED_CMD); // set segment remap
  oled_write(0xA6, OLED_CMD); // normal / reverse
  oled_write(0xA8, OLED_CMD); // set multiplex ratio(1 to 64)
  oled_write(0x3F, OLED_CMD); // 1/32 duty
  oled_write(0xC8, OLED_CMD); // Com scan direction
  oled_write(0xD3, OLED_CMD); // set display offset
  oled_write(0x00, OLED_CMD); //

  oled_write(0xD5, OLED_CMD); // set osc division
  oled_write(0x80, OLED_CMD); //

  oled_write(0xD8, OLED_CMD); // set area color mode off
  oled_write(0x05, OLED_CMD); //

  oled_write(0xD9, OLED_CMD); // Set Pre-Charge Period
  oled_write(0xF1, OLED_CMD); //

  oled_write(0xDA, OLED_CMD); // set com pin configuartion
  oled_write(0x12, OLED_CMD); //

  oled_write(0xDB, OLED_CMD); // set Vcomh
  oled_write(0x30, OLED_CMD); //

  oled_write(0x8D, OLED_CMD); // set charge pump enable
  oled_write(0x14, OLED_CMD); //

  oled_write(0xAF, OLED_CMD); // turn on oled panel
}
