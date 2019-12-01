/**
 * @file producto.cpp
 * Producto!
 */

////////////////////////////////////////////////////////////////////////////////

#include "display.h"
#include "producto.h"
#include "Arduino.h"
#include <Button2.h>
#include <TFT_eSPI.h>

////////////////////////////////////////////////////////////////////////////////

static Producto producto = {
  .buttons = {
    {
     .btn = Button2(PRODUCTO_BTN_START),
     .str = "LIST",
     .id = 0x69,
     .pin = PRODUCTO_BTN_START,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_STOP),
     .str = "PAUSE/RESUME",
     .id = 0x70,
     .pin = PRODUCTO_BTN_STOP,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_1),
     .str = "Pooping",
     .id = 1,
     .pin = PRODUCTO_BTN_TASK_1,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_2),
     .str = "Peeing",
     .id = 2,
     .pin = PRODUCTO_BTN_TASK_2,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_3),
     .str = "Coffee butt",
     .id = 3,
     .pin = PRODUCTO_BTN_TASK_3,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_4),
     .str = "Busy",
     .id = 4,
     .pin = PRODUCTO_BTN_TASK_4,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_5),
     .str = "Getting busy",
     .id = 5,
     .pin = PRODUCTO_BTN_TASK_5,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_BTN_TASK_6),
     .str = "Crackin' knuckles",
     .id = 6,
     .pin = PRODUCTO_BTN_TASK_6,
     .timer = 0,
    },
  },
  .num_buttons = PRODUCTO_BTNS,
  .active_timer = 0,
  .paused_timer = 0,
  .tft = TFT_eSPI(135, 240),
};

////////////////////////////////////////////////////////////////////////////////

static void serial_init();
static void button_init();
static void check_buttons();
static void task_btn(int btn_num);
static void toggle_list_tasks(Button2&);
static void button_isr(int i);
static void pause_resume_timer(Button2&);
static void do_timer();

////////////////////////////////////////////////////////////////////////////////

static void task_btn1(Button2&) { task_btn(2); }
static void task_btn2(Button2&) { task_btn(3); }
static void task_btn3(Button2&) { task_btn(4); }
static void task_btn4(Button2&) { task_btn(5); }
static void task_btn5(Button2&) { task_btn(6); }
static void task_btn6(Button2&) { task_btn(7); }

static void handle0() { button_isr(0); }
static void handle1() { button_isr(1); }
static void handle2() { button_isr(2); }
static void handle3() { button_isr(3); }
static void handle4() { button_isr(4); }
static void handle5() { button_isr(5); }
static void handle6() { button_isr(6); }
static void handle7() { button_isr(7); }

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Producto
 */
void producto_init()
{
  display_init(&producto);
  serial_init();
  button_init();
}

/**
 * Intended to be run on each main loop iteration
 */
void producto_loop()
{
  // check_buttons();
  do_timer();
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the serial port
 */
static void serial_init()
{
  Serial.begin(115200);
  Serial.println("Start");
  Serial.flush();
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Pause/resume timer
 */
static void pause_resume_timer(Button2&)
{
  if (producto.active_timer == 0) {
    producto.active_timer = producto.paused_timer;
    producto.paused_timer = 0;
  } else {
    Serial.println("PAUSE TIMER");
    producto.paused_timer = producto.active_timer;
    producto.active_timer = 0;
  }
}

/**
 * If the timer is active, do timer things
 */
static void do_timer()
{
  static int prev_millis = 0;
  int time_elapsed = millis() - prev_millis;

  if (producto.active_timer > 0 && time_elapsed > 1000) {
    producto.buttons[producto.active_timer].timer++;
    // TODO: only do this on interrupt
    display_draw();
    prev_millis = millis();
  }
}

////////////////////////////////////////////////////////////////////////////////


static void button_isr(int i)
{
  producto.buttons[i].btn.loop();
}

/**
 * Initialize buttons with callback functions
 */
static void button_init()
{
  ProductoButton *buttons = producto.buttons;
  
  buttons[0].btn.setTapHandler(toggle_list_tasks);
  buttons[1].btn.setTapHandler(pause_resume_timer);
  buttons[2].btn.setTapHandler(task_btn1);
  buttons[3].btn.setTapHandler(task_btn2);
  buttons[4].btn.setTapHandler(task_btn3);
  buttons[5].btn.setTapHandler(task_btn4);
  buttons[6].btn.setTapHandler(task_btn5);
  buttons[7].btn.setTapHandler(task_btn6);

  attachInterrupt(buttons[0].pin, handle0, CHANGE);
  attachInterrupt(buttons[1].pin, handle1, CHANGE);
  attachInterrupt(buttons[2].pin, handle2, CHANGE);
  attachInterrupt(buttons[3].pin, handle3, CHANGE);
  attachInterrupt(buttons[4].pin, handle4, CHANGE);
  attachInterrupt(buttons[5].pin, handle5, CHANGE);
  attachInterrupt(buttons[6].pin, handle6, CHANGE);
  attachInterrupt(buttons[7].pin, handle7, CHANGE);
}

/**
 * Handle each Producto button
 */
static void check_buttons()
{
  for (int i = 0; i < PRODUCTO_BTNS; i++) {
    producto.buttons[i].btn.loop();
  }
}

static void task_btn(int btn_num)
{
  ProductoButton button = producto.buttons[btn_num];

  producto.active_timer = btn_num;
  producto.paused_timer = 0;

  display_transition(TASK);
}

static void toggle_list_tasks(Button2&)
{
  static bool showing_list = false;

  if (showing_list == false) {
    display_transition(LIST);
    showing_list = true;
  } else {
    display_transition(TASK);
    showing_list = false;
  }
}
