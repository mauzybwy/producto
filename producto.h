/**
 * @file producto.h
 * Producto!
 */
#ifndef _PRODUCTO_H_
#define _PRODUCTO_H_

#include "Arduino.h"
#include <Button2.h>
#include <TFT_eSPI.h>
#include <RTClib.h>

#define PRODUCTO_TASK_FILE "/producto_tasks.txt"
#define PRODUCTO_ACTIVE_FILE "/producto_active_task.txt"

#define PRODUCTO_TASKS 6
#define PRODUCTO_TASK_0_BTN 13
#define PRODUCTO_TASK_1_BTN 12
#define PRODUCTO_TASK_2_BTN 15
#define PRODUCTO_TASK_3_BTN 2
#define PRODUCTO_TASK_4_BTN 17
#define PRODUCTO_TASK_5_BTN 32
#define PRODUCTO_TASK_NONE 0xFF

#define PRODUCTO_BTNS 2
#define PRODUCTO_BTN_START 35
#define PRODUCTO_BTN_STOP 0

#define PRODUCTO_RTC_SQW_PIN 33

void producto_init();
void producto_loop();

typedef struct producto_button_s {
  Button2 btn;
  String str;
  byte id;
  byte pin;
  unsigned int timer;
} ProductoButton;

typedef ProductoButton ProductoTask;

typedef struct producto_s {
  ProductoButton tasks[PRODUCTO_TASKS];
  ProductoButton buttons[PRODUCTO_BTNS];
  byte num_tasks;
  byte num_buttons;
  byte active_task;
  byte paused_task;
  TFT_eSPI tft;
  RTC_DS3231 rtc;
  DateTime start_time;
} Producto;

#endif // _PRODUCTO_H_
