/**
 * @file producto.cpp
 * Producto!
 */

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "producto.h"
#include "Arduino.h"
#include <Button2.h>
#include <TFT_eSPI.h>

////////////////////////////////////////////////////////////////////////////////
// Local
////////////////////////////////////////////////////////////////////////////////

static TFT_eSPI tft = TFT_eSPI(135, 240);

typedef struct producto_button_s {
  Button2 btn;
  String str;
  byte id;
  byte pin;
  unsigned int timer;
} ProductoButton;

static ProductoButton buttons[PRODUCTO_BTNS] = {
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
};

static bool screen_list_lock = false;

////////////////////////////////////////////////////////////////////////////////

typedef enum display_state_e {
  START = 0,
  TASK,
  LIST,
  TIMER,
} DisplayState;

////////////////////////////////////////////////////////////////////////////////

static void serial_init();

static void tft_init();
static void tft_clr_wr(String str, int w = tft.width() / 2, int h = tft.height() / 2);
static void producto_circle(int task_num);
static void producto_wr_task(String task_str);
static void producto_wr_timer(int time);

static DisplayState display_state = START;
static byte active_timer = 0;
static byte paused_timer = 0;
static void pause_resume_timer(Button2&);
static void do_timer();

static void update_display();

static void button_init();
static void check_buttons();
static void test1(Button2&);
static void test2(Button2&);
static void test3(Button2&);
static void test4(Button2&);
static void test5(Button2&);
static void test6(Button2&);
static void task_btn(int btn_num);
static void producto_list_tasks();
static void toggle_list_tasks(Button2&);

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Producto
 */
void producto_init()
{
  serial_init();
  tft_init();
  button_init();

  display_state = LIST;
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
 * Initialize the TFT screen
 */
static void tft_init()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);

  /* Turn on backlight (necessary to see screen) */
  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  }

  tft.setSwapBytes(true);

  producto_circle(0);
  // tft.fillScreen(TFT_RED);
}

static void tft_clr_wr(String str, int w, int h)
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(str, w, h);

  // for (int i = 0; i < 6; i++) {
  //   // tft.drawCircle(20 + (40 * i), 10, 5, TFT_WHITE);
  //   tft.fillCircle(20 + (40 * i), 10, 2, TFT_WHITE);
  // }
}

static void producto_circle(int task_num)
{
  // tft.drawRect(1, 1, tft.width() - 2, 20, TFT_WHITE);
  tft.fillRect(1, 1, tft.width() - 2, 18, TFT_BLACK);

  for (int i = 0; i < 6; i++) {
    // tft.drawCircle(20 + (40 * i), 10, 5, TFT_WHITE);
    tft.fillCircle(20 + (40 * i), 10, 2, TFT_WHITE);
  }

  if (task_num > 0) {
    tft.drawCircle((task_num * 40) - 20, 10, 6, TFT_WHITE);
  }
}

static void producto_wr_task(String task_str)
{
  if (screen_list_lock == false) {
    tft.setTextSize(2);  
    // tft.drawRect(1, 34, tft.width() - 2, 20, TFT_WHITE);
    // tft.fillRect(1, 34, tft.width() - 2, 20, TFT_BLACK); // JUST THE TASK STRING
    tft.fillRect(1, 20, tft.width() - 2, tft.height(), TFT_BLACK);
    tft.drawString(task_str, tft.width()/2, 45);
    tft.setTextSize(1);
  }
}

static void producto_wr_timer(int time)
{
  if (screen_list_lock == false) {
    byte hr = time / 3600;
    byte min = time / 60;
    byte sec = time % 60;

    char time_str[9];
    sprintf(time_str, "%02d:%02d:%02d", hr, min, sec);
    // String time_str = hr + (String)":" + min + (String)":" + sec;

    tft.setTextSize(5);
    // tft.drawRect(1, 72, tft.width() - 2, 41, TFT_WHITE);
    // tft.fillRect(1, 72, tft.width() - 2, 41, TFT_BLACK);
    tft.drawString(time_str, tft.width()/2, 95);
    tft.setTextSize(1);
  }
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Pause/resume timer
 */
static void pause_resume_timer(Button2&)
{

  if (active_timer == 0) {
    active_timer = paused_timer;
    paused_timer = 0;
  } else {
    Serial.println("PAUSE TIMER");
    paused_timer = active_timer;
    active_timer = 0;
  }
}

/**
 * If the timer is active, do timer things
 */
static void do_timer()
{
  static int prev_millis = 0;
  int time_elapsed = millis() - prev_millis;

  if (active_timer > 0 && time_elapsed > 1000) {
    buttons[active_timer].timer++;
    // TODO: only do this on interrupet
    update_display();
    prev_millis = millis();
  }
}


////////////////////////////////////////////////////////////////////////////////

static void display_start()
{
  producto_list_tasks();
}

static void display_task()
{
  ProductoButton *button;

  if (active_timer > 0) {
    button = &buttons[active_timer];
  } else if (paused_timer > 0) {
    button = &buttons[paused_timer];
  } else {
    button = NULL;
  }

  if (button != NULL) {
    producto_wr_task(button->str);
    producto_wr_timer(button->timer);
    producto_circle(button->id);

    display_state = TIMER;
  } else {
    producto_list_tasks();
  }
}

static void display_list()
{
  producto_list_tasks();
}

static void display_timer()
{
  ProductoButton *button;
  button = active_timer > 0 ? &buttons[active_timer] : &buttons[paused_timer];
  producto_wr_timer(button->timer);
}

static void update_display()
{
  switch (display_state) {
  case START:
    display_start();
    break;

  case TASK:
    display_task();
    break;

  case LIST:
    display_list();
    break;

  case TIMER:
    display_timer();
    break;

  default:
    break;
  }
}

static void button_isr(int i)
{
  buttons[i].btn.loop();
  update_display();
}

static void handle0() { button_isr(0); }
static void handle1() { button_isr(1); }
static void handle2() { button_isr(2); }
static void handle3() { button_isr(3); }
static void handle4() { button_isr(4); }
static void handle5() { button_isr(5); }
static void handle6() { button_isr(6); }
static void handle7() { button_isr(7); }

/**
 * Initialize buttons with callback functions
 */
static void button_init()
{
  buttons[0].btn.setTapHandler(toggle_list_tasks);
  buttons[1].btn.setTapHandler(pause_resume_timer);

  buttons[2].btn.setTapHandler(test1);
  buttons[3].btn.setTapHandler(test2);
  buttons[4].btn.setTapHandler(test3);
  buttons[5].btn.setTapHandler(test4);
  buttons[6].btn.setTapHandler(test5);
  buttons[7].btn.setTapHandler(test6);

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
    buttons[i].btn.loop();
  }
}

static void test1(Button2&)
{
  task_btn(2);
}

static void test2(Button2&)
{
  task_btn(3);
}

static void test3(Button2&)
{
  task_btn(4);
}

static void test4(Button2&)
{
  task_btn(5);
}

static void test5(Button2&)
{
  task_btn(6);
}

static void test6(Button2&)
{
  task_btn(7);
}

static void task_btn(int btn_num)
{
  ProductoButton button = buttons[btn_num];

  active_timer = btn_num;
  paused_timer = 0;
  screen_list_lock = false;

  display_state = TASK;
}

static void producto_list_tasks()
{
  ProductoButton *button;
  int y_offset;

  tft.fillRect(0, 20, tft.width(), tft.height(), TFT_BLACK);

  tft.setRotation(2);
  tft.setTextSize(1);
  tft.setTextDatum(ML_DATUM);

  for (int i = 2; i < 8; i++) {
    button = &buttons[i];
    y_offset = 240 - ((40 * button->id) - 20);
    tft.drawString(button->str, 20, y_offset);
  }

  tft.setRotation(1);
  tft.setTextDatum(MC_DATUM);
}

static void toggle_list_tasks(Button2&)
{
  static bool showing_list = false;

  if (showing_list == false) {
    display_state = LIST;
    showing_list = true;
  } else {
    display_state = TASK;
    showing_list = false;
  }
}
