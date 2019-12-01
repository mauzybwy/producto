/**
 * @file producto.h
 * Producto!
 */
#ifndef _PRODUCTO_H_
#define _PRODUCTO_H_

#include "Arduino.h"
#include <Button2.h>
#include <TFT_eSPI.h>

#define PRODUCTO_BTNS 8
#define PRODUCTO_BTN_START 35
#define PRODUCTO_BTN_STOP 0
#define PRODUCTO_BTN_TASK_1 13
#define PRODUCTO_BTN_TASK_2 12
#define PRODUCTO_BTN_TASK_3 15
#define PRODUCTO_BTN_TASK_4 2
#define PRODUCTO_BTN_TASK_5 17
#define PRODUCTO_BTN_TASK_6 32

void producto_init();
void producto_loop();

typedef struct producto_button_s {
  Button2 btn;
  String str;
  byte id;
  byte pin;
  unsigned int timer;
} ProductoButton;

typedef struct producto_s {
  ProductoButton buttons[PRODUCTO_BTNS];
  byte num_buttons;
  byte active_timer;
  byte paused_timer;
  TFT_eSPI tft;
} Producto;

#endif // _PRODUCTO_H_
