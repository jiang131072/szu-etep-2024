#ifndef __UTIL_H
#define __UTIL_H

#include <STC89C5xRC.H>

// stdint.h
#define uint8_t unsigned char
#define uint16_t unsigned short int
#define uint32_t unsigned long int

// Some Pin Definitions
sfr SEG_DISPLAY = 0x80;

sbit SEG1 = P2 ^ 3;
sbit SEG2 = P2 ^ 2;
sbit SEG3 = P2 ^ 1;
sbit SEG4 = P2 ^ 0;

sbit KEY1 = P3 ^ 2;
sbit KEY2 = P3 ^ 3;
sbit KEY3 = P3 ^ 4;
sbit LED1 = P2 ^ 4;
sbit LED2 = P2 ^ 5;
sbit LED3 = P2 ^ 6;

sbit BEEP = P1^0;

// Digit to 7-segment LED Display
const uint8_t digit2seg[];

#define SEL_SEG1() SEG1 = 0
#define SEL_SEG2() SEG2 = 0
#define SEL_SEG3() SEG3 = 0
#define SEL_SEG4() SEG4 = 0

#define UNSEL_SEG1() SEG1 = 1
#define UNSEL_SEG2() SEG2 = 1
#define UNSEL_SEG3() SEG3 = 1
#define UNSEL_SEG4() SEG4 = 1

#define BEEP_ON() BEEP = 0
#define BEEP_OFF() BEEP = 1

void delay_ms(uint16_t n);

// Macros

#endif