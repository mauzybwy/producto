#include "producto.h"
#include "display.h"

////////////////////////////////////////////////////////////////////////////////

DisplayState display_state;
static Producto *producto;

////////////////////////////////////////////////////////////////////////////////

static void tft_init(TFT_eSPI *tft);
static void producto_circle(int task_num);
static void producto_wr_timer(int time);
static void producto_wr_task(String task_str);
static void producto_list_tasks();
static void display_start();
static void display_task();
static void display_list();
static void display_timer();

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Display
 */
void display_init(Producto *p)
{
  producto = p;
  tft_init(&producto->tft);
  display_transition(START);
}

/**
 * Transistion between Display states
 */
void display_transition(DisplayState state)
{
  display_state = state;

  switch (display_state) {
  case START:
    producto->tft.setTextDatum(ML_DATUM);
    producto->tft.setTextSize(1);
    producto->tft.setRotation(2);
    break;

  case TASK:
    producto->tft.setRotation(1);
    producto->tft.fillRect(0, 20, producto->tft.width(), producto->tft.height(), TFT_BLACK);
    producto->tft.setTextDatum(MC_DATUM);
    producto->tft.setTextSize(2);
    break;

  case LIST:
    producto->tft.setRotation(1);
    producto->tft.fillRect(0, 20, producto->tft.width(), producto->tft.height(), TFT_BLACK);
    producto->tft.setRotation(2);
    producto->tft.setTextDatum(ML_DATUM);
    producto->tft.setTextSize(1);
    break;

  case TIMER:
    producto->tft.setRotation(1);
    producto->tft.setTextDatum(MC_DATUM);
    producto->tft.setTextSize(5);
    break;

  default:
    break;
  }

  display_draw();
}

/**
 * Actually draw the display
 */
void display_draw()
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

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the TFT screen
 */
static void tft_init(TFT_eSPI *tft)
{
  tft->init();
  tft->setRotation(1);
  tft->fillScreen(TFT_BLACK);
  tft->setTextSize(2);
  tft->setTextColor(TFT_WHITE, TFT_BLACK);
  tft->setCursor(0, 0);
  tft->setTextDatum(MC_DATUM);
  tft->setTextSize(1);

  /* Turn on backlight (necessary to see screen) */
  if (TFT_BL > 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  }

  tft->setSwapBytes(true);  
}

static void producto_circle(int task_num)
{
  producto->tft.fillRect(1, 1, producto->tft.width() - 2, 18, TFT_BLACK);

  for (int i = 0; i < 6; i++) {
    producto->tft.fillCircle(20 + (40 * i), 10, 2, TFT_WHITE);
  }

  if (task_num > 0) {
    producto->tft.drawCircle((task_num * 40) - 20, 10, 6, TFT_WHITE);
  }
}

static void producto_wr_timer(int time)
{

  byte hr = time / 3600;
  byte min = time / 60;
  byte sec = time % 60;

  char time_str[9];
  sprintf(time_str, "%02d:%02d:%02d", hr, min, sec);

  producto->tft.drawString(time_str, producto->tft.width()/2, 95);  
}

static void producto_wr_task(String task_str)
{
  producto->tft.drawString(task_str, producto->tft.width()/2, 45);
}

static void producto_list_tasks()
{
  ProductoButton *button;
  int y_offset;

  for (int i = 2; i < 8; i++) {
    button = &producto->buttons[i];
    y_offset = 240 - ((40 * button->id) - 20);
    producto->tft.drawString(button->str, 20, y_offset);
  }
}

static void display_start()
{
  
  producto_list_tasks();
  
  producto->tft.setRotation(1);
  producto_circle(0);
}

static void display_task()
{
  ProductoButton *button;
  
  if (producto->active_timer > 0) {
    button = &producto->buttons[producto->active_timer];
  } else if (producto->paused_timer > 0) {
    button = &producto->buttons[producto->paused_timer];
  } else {
    button = NULL;
  }

  if (button != NULL) {
    producto_wr_task(button->str);
    producto_circle(button->id);

    display_transition(TIMER);
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
  button = producto->active_timer > 0 ? &producto->buttons[producto->active_timer] : &producto->buttons[producto->paused_timer];
  producto_wr_timer(button->timer);
}
