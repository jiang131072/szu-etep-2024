#ifndef __OLED_H
#define __OLED_H

#include <STC89C5xRC.H>
#include "util.h"

// OLED Pin Definitions
sbit OLED_SCL = P4 ^ 0; // SCL
sbit OLED_SDA = P3 ^ 7; // SDA

// OLED Definitions
#define OLED_CMD 0  // 写命令
#define OLED_DATA 1 // 写数据

#define MAX_COL 128
#define MAX_ROW 64

// OLED Control Functions
#define oled_scl_clr() OLED_SCL = 0
#define oled_scl_set() OLED_SCL = 1

#define oled_sda_clr() OLED_SDA = 0
#define oled_sda_set() OLED_SDA = 1

void oled_init(void);
void oled_clear(void);
void oled_fill(uint8_t byte);
void oled_goto(uint8_t x, uint8_t y);
void oled_write(uint8_t byte, bit type);

// OLED Display Functions
void oled_show_ascii(uint8_t x, uint8_t y, uint8_t character);
void oled_show_colomn(uint8_t page, uint8_t colomn, uint8_t byte);
void oled_show_ascii_string(uint8_t x, uint8_t y, uint8_t *string);
void oled_show_uint32_t(uint8_t x, uint8_t y, uint32_t num, uint8_t len);

// IIC Functions
void iic_start();
void iic_stop();
void iic_wait_ack();
void iic_send_byte(uint8_t byte);
void iic_write_data(uint8_t byte);
void iic_write_command(uint8_t byte);
#endif
