#include <intrins.h>
#include <stdlib.h>

#include <STC89C5xRC.H>

#include "oled.h"
#include "util.h"

#define COOLDOWN 12

uint8_t get_black_block(void);
bit update_board(void);
void init_board(void);
void show_board(void);
void show_board_background(void);
void show_game_over(void);
void show_game_win(void);
void switch_key_state(void);

bit check_2048(void);

void merge_arr(uint8_t arr[4]);
void merge(uint8_t direc);

bit key_state = 1;

bit updated = 0;

uint16_t cooldown = 0;
uint16_t board[4][4] = {0};

uint8_t black_block_y[16];
uint8_t black_block_x[16];

volatile bit key1_clicked = 0;
volatile bit key2_clicked = 0;

void int_init() {
  // 使能
  EX0 = 1;
  EX1 = 1;

  // 下降沿触发
  IT0 = 1;
  IT1 = 1;

  // 总中断
  EA = 1;
}
void int_key1() interrupt 0 {
  if (cooldown == 0)
    key1_clicked = 1;
}
void int_key2() interrupt 2 {
  if (cooldown == 0)
    key2_clicked = 1;
}

// 2048 Game
int main(void) {
  uint16_t step = 0;
  uint8_t s3, s2, s1, s0;

  int_init();

  oled_init();
  delay_ms(50);
  oled_clear();

  init_board();
  show_board_background();
  show_board();

  while (1) {
    if (KEY3 == 0 && cooldown == 0) {
      switch_key_state();
      cooldown = COOLDOWN;
    }

    if (key1_clicked == 1) {
      merge(key_state ? 2 : 0);
      updated = 1;

      cooldown = COOLDOWN;
      key1_clicked = 0;
    }

    if (key2_clicked == 1) {
      merge(key_state ? 3 : 1);
      updated = 1;

      cooldown = COOLDOWN;
      key2_clicked = 0;
    }

    if (updated == 1) {
      if (update_board() == 1) {
        show_game_over();
        for (step = 0; step < 100; step++) {
          BEEP_ON();
          delay_ms(1);
          BEEP_OFF();
          delay_ms(1);
        }
        while (1)
          ;
      } else if (check_2048() == 1) {
        show_game_win();
        for (step = 0; step < 100; step++) {
          BEEP_ON();
          delay_ms(1);
          BEEP_OFF();
          delay_ms(1);
        }
        while (1)
          ;
      }
      show_board();
      step++;

      updated = 0;
    }

    s0 = step % 10;
    s1 = (step / 10) % 10;
    s2 = (step / 100) % 10;
    s3 = (step / 1000) % 10;

    SEG_DISPLAY = digit2seg[s3];
    SEL_SEG1();
    delay_ms(5);
    UNSEL_SEG1();

    SEG_DISPLAY = digit2seg[s2];
    SEL_SEG2();
    delay_ms(5);
    UNSEL_SEG2();

    SEG_DISPLAY = digit2seg[s1];
    SEL_SEG3();
    delay_ms(5);
    UNSEL_SEG3();

    SEG_DISPLAY = digit2seg[s0];
    SEL_SEG4();
    delay_ms(5);
    UNSEL_SEG4();

    if (cooldown > 0) {
      cooldown--;
    }
  }
}

void switch_key_state(void) {
  key_state = ~key_state;
  if (key_state == 0) {
    oled_show_ascii_string(108, 2, "Up");
    oled_show_ascii_string(108, 6, "Dw");
  } else {
    oled_show_ascii_string(108, 2, "Le");
    oled_show_ascii_string(108, 6, "Ri");
  }
}

uint8_t get_black_block(void) {
  uint8_t y, x, i = 0;
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      if (board[y][x] == 0) {
        black_block_y[i] = y;
        black_block_x[i] = x;
        i++;
      }
    }
  }
  return i;
}

// if return 1, then game over
bit update_board(void) {
  uint8_t n = get_black_block(), i;
  if (n == 0) {
    return 1;
  }

  i = rand() % (n - 1);
  board[black_block_y[i]][black_block_x[i]] = (rand() > 29490) ? 4 : 2;
  return 0;
}

void init_board(void) {
  update_board();
  update_board();
}

void show_board(void) {
  uint8_t y, x;
  for (y = 0; y < 4; y += 1) {
    for (x = 0; x < 4; x += 1) {
      oled_show_uint32_t(x * 25, y * 2 + 1, board[y][x], 4);
    }
  }
}

void show_board_background(void) {
  uint8_t p, c;

  // 棋盘横格
  for (p = 2; p < 8; p += 2) {
    for (c = 0; c < 100; c++) {
      oled_show_colomn(p, c, 0x01);
    }
  }

  //  棋盘竖格
  for (p = 0; p < 8; p += 1) {
    for (c = 24; c < 100; c += 25) {
      oled_show_colomn(p, c, 0xff);
    }
  }

  // 分隔横线
  for (c = 100; c < 128; c++) {
    oled_show_colomn(4, c, 0x01);
  }

  // 按键提示
  oled_show_ascii_string(108, 1, "K1");
  oled_show_ascii_string(108, 5, "K2");

  // 默认两个为上下状态
  switch_key_state();
}

void show_game_over(void) {
  oled_clear();
  oled_show_ascii_string(18, 3, "Game Over :(");
}

void show_game_win(void) {
  oled_clear();
  oled_show_ascii_string(24, 3, "You Win :)");
}

bit check_2048(void) {
  uint8_t y, x;
  for (y = 0; y < 4; y++) {
    for (x = 0; x < 4; x++) {
      if (board[y][x] == 2048) {
        return 1;
      }
    }
  }
  return 0;
}

void merge_arr(uint8_t arr[4]) {
  int i, j;
  // Slide non-zero values to the front
  for (i = 0; i < 4; i++) {
    if (arr[i] == 0) {
      for (j = i + 1; j < 4; j++) {
        if (arr[j] != 0) {
          arr[i] = arr[j];
          arr[j] = 0;
          break;
        }
      }
    }
  }

  // Merge adjacent equal values
  for (i = 0; i < 4 - 1; i++) {
    if (arr[i] != 0 && arr[i] == arr[i + 1]) {
      arr[i] *= 2;
      arr[i + 1] = 0;
    }
  }

  // Slide again to fill empty spaces after merging
  for (i = 0; i < 4; i++) {
    if (arr[i] == 0) {
      for (j = i + 1; j < 4; j++) {
        if (arr[j] != 0) {
          arr[i] = arr[j];
          arr[j] = 0;
          break;
        }
      }
    }
  }
}

void merge(uint8_t direc) {
  uint8_t i, j, arr[4];

  switch (direc) {
  case 0: // Up
    for (j = 0; j < 4; j++) {
      for (i = 0; i < 4; i++) {
        arr[i] = board[i][j];
      }
      merge_arr(arr);
      for (i = 0; i < 4; i++) {
        board[i][j] = arr[i];
      }
    }
    break;
  case 1: // Down
    for (j = 0; j < 4; j++) {
      for (i = 0; i < 4; i++) {
        arr[i] = board[4 - 1 - i][j];
      }
      merge_arr(arr);
      for (i = 0; i < 4; i++) {
        board[4 - 1 - i][j] = arr[i];
      }
    }
    break;
  case 2: // Left
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        arr[j] = board[i][j];
      }
      merge_arr(arr);
      for (j = 0; j < 4; j++) {
        board[i][j] = arr[j];
      }
    }
    break;
  case 3: // Right
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        arr[j] = board[i][4 - 1 - j];
      }
      merge_arr(arr);
      for (j = 0; j < 4; j++) {
        board[i][4 - 1 - j] = arr[j];
      }
    }
    break;
  }
}