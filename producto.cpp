/**
 * @file producto.cpp
 * Producto!
 */

////////////////////////////////////////////////////////////////////////////////

#include "producto.h"
#include "logging.h"
#include "myspiffs.h"
#include "display.h"
#include "commands.h"
#include "shared_mem.h"
#include "server.h"

#include "Arduino.h"
#include <Button2.h>
#include <TFT_eSPI.h>
#include <RTClib.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiMulti.h> 
// #include <mdns.h>

////////////////////////////////////////////////////////////////////////////////

static Producto producto = {
  .tasks = {
    {
     .btn = Button2(PRODUCTO_TASK_0_BTN),
     .str = "Pooping",
     .id = 1,
     .pin = PRODUCTO_TASK_0_BTN,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_TASK_1_BTN),
     .str = "Peeing",
     .id = 2,
     .pin = PRODUCTO_TASK_1_BTN,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_TASK_2_BTN),
     .str = "Coffee butt",
     .id = 3,
     .pin = PRODUCTO_TASK_2_BTN,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_TASK_3_BTN),
     .str = "Busy",
     .id = 4,
     .pin = PRODUCTO_TASK_3_BTN,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_TASK_4_BTN),
     .str = "Getting busy",
     .id = 5,
     .pin = PRODUCTO_TASK_4_BTN,
     .timer = 0,
    },
    {
     .btn = Button2(PRODUCTO_TASK_5_BTN),
     .str = "Crackin' knuckles",
     .id = 6,
     .pin = PRODUCTO_TASK_5_BTN,
     .timer = 0,
    },
  },
  
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
  },

  .num_tasks = PRODUCTO_TASKS,
  .num_buttons = PRODUCTO_BTNS,
  .active_task = PRODUCTO_TASK_NONE,
  .paused_task = PRODUCTO_TASK_NONE,
  .tft = TFT_eSPI(135, 240),
  .rtc = RTC_DS3231(),
};

////////////////////////////////////////////////////////////////////////////////

static bool save_active_state_flag = false;
static int task_pressed = PRODUCTO_TASKS;
static int button_pressed = PRODUCTO_BTNS;

////////////////////////////////////////////////////////////////////////////////

static void serial_init();
static void rtc_init();
static void task_init();
static void button_init();
static void wifi_init();
static void task_btn(int task_num);
static void toggle_list_tasks(Button2&);
static void check_task_press();
static void check_button_press();
static void pause_resume_timer(Button2&);
static void IRAM_ATTR do_timer();
static void save_active_state();
static void handle_serial_commands();
static void append_active_to_tasks_file();

////////////////////////////////////////////////////////////////////////////////

static void task_btn0(Button2&) { task_btn(0); }
static void task_btn1(Button2&) { task_btn(1); }
static void task_btn2(Button2&) { task_btn(2); }
static void task_btn3(Button2&) { task_btn(3); }
static void task_btn4(Button2&) { task_btn(4); }
static void task_btn5(Button2&) { task_btn(5); }

static void IRAM_ATTR task_handle0() { task_pressed = 0; }
static void IRAM_ATTR task_handle1() { task_pressed = 1; }
static void IRAM_ATTR task_handle2() { task_pressed = 2; }
static void IRAM_ATTR task_handle3() { task_pressed = 3; }
static void IRAM_ATTR task_handle4() { task_pressed = 4; }
static void IRAM_ATTR task_handle5() { task_pressed = 5; }

static void IRAM_ATTR button_handle0() { button_pressed = 0; }
static void IRAM_ATTR button_handle1() { button_pressed = 1; }

////////////////////////////////////////////////////////////////////////////////

WiFiMulti wifiMulti;

/* Wifi Config */
#define NUM_WIFI (1)
const char* ssid[NUM_WIFI] = {
    "TheSaltySpitoon_2GEXT"
    /* "TheSaltySpitoon", */
};

const char* password[NUM_WIFI] = {    
    "AbandonAllHopeYeWhoEnterHere",
    /* "AbandonAllHopeYeWhoEnterHere" */
};

IPAddress ip(192, 168, 1, 77);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the Producto
 */
void producto_init()
{
  String path,fname, line;

  serial_init();
  wifi_init();
  server_init(&producto);

  noInterrupts();

  myspiffs_init();

  path = "/task/";
  for (int i = 0; i < producto.num_tasks; i++) {
    fname = path + producto.tasks[i].id;
    fname.toCharArray(big_arr, BIG);
    
    if (!myspiffs_file_exists(big_arr)) {
      producto.tasks[i].str.toCharArray(very_big_arr, VERY_BIG);
      myspiffs_write_file(big_arr, very_big_arr, true);
    }
    
    line = myspiffs_read_first_line_of_file(big_arr);
    producto.tasks[i].str = String(line);
  }

  display_init(&producto);
  task_init();
  button_init();
  //rtc_init();
  producto.start_time = producto.rtc.now();

  interrupts();

  server_start();

  append_active_to_tasks_file();
}

/**
 * Intended to be run on each main loop iteration
 */
void producto_loop()
{
  handle_serial_commands();
  if (save_active_state_flag) {
    save_active_state_flag = false;
    save_active_state();
  }

  server_check_requests();
  
  check_task_press();
  check_button_press();
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Initialize the serial port
 */
static void serial_init()
{
  Serial.begin(115200);
  Serial.println("Start");
  ERR_PRINTLN("ERR ENABLE");
  DEBUG_PRINTLN("DEBUG ENABLE");
  Serial.flush();
}

////////////////////////////////////////////////////////////////////////////////

static void rtc_init()
{
  /* UH OH */
  if (! producto.rtc.begin()) {
    ERR_PRINTLN("Couldn't find RTC");
    while (1);
  }

  /* Reset the time if RTC loses power */
  if (producto.rtc.lostPower()) {
    ERR_PRINTLN("RTC lost power, lets set the time!");
    producto.rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // January 21, 2014 at 3am
  }

  /* Configure the RTC to run a 1HZ square wave for our clocking */
  producto.rtc.writeSqwPinMode(DS3231_SquareWave1Hz);

  /* Set up an interrupt on the square wave pin to do the timer thing */
  pinMode(PRODUCTO_RTC_SQW_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PRODUCTO_RTC_SQW_PIN), do_timer, RISING);
}

/**
 * Pause/resume timer
 */
static void pause_resume_timer(Button2&)
{
  if (producto.active_task == PRODUCTO_TASK_NONE) {
    producto.active_task = producto.paused_task;
    producto.paused_task = PRODUCTO_TASK_NONE;
  } else {
    DEBUG_PRINTLN("PAUSE TIMER");
    producto.paused_task = producto.active_task;
    producto.active_task = PRODUCTO_TASK_NONE;
  }
}

/**
 * If the timer is active, do timer things
 */
static void IRAM_ATTR do_timer()
{
  static int total_seconds = 0;
  
  if (producto.active_task != PRODUCTO_TASK_NONE) {
    producto.tasks[producto.active_task].timer++;
    display_draw();
  }

  // if (total_seconds % 60 == 0) {
    save_active_state_flag = true;
  // }
}

////////////////////////////////////////////////////////////////////////////////


static void check_button_press()
{
  if (button_pressed < PRODUCTO_BTNS) {
    producto.buttons[button_pressed].btn.loop();
  }
}

static void check_task_press()
{
  if (task_pressed < PRODUCTO_TASKS) {
    producto.tasks[task_pressed].btn.loop();
  }
}

/**
 * Initialize tasks with callback functions
 */
static void task_init()
{
  ProductoTask *tasks = producto.tasks;
  
  tasks[0].btn.setTapHandler(task_btn0);
  tasks[1].btn.setTapHandler(task_btn1);
  tasks[2].btn.setTapHandler(task_btn2);
  tasks[3].btn.setTapHandler(task_btn3);
  tasks[4].btn.setTapHandler(task_btn4);
  tasks[5].btn.setTapHandler(task_btn5);
  
  attachInterrupt(tasks[0].pin, task_handle0, CHANGE);
  attachInterrupt(tasks[1].pin, task_handle1, CHANGE);
  attachInterrupt(tasks[2].pin, task_handle2, CHANGE);
  attachInterrupt(tasks[3].pin, task_handle3, CHANGE);
  attachInterrupt(tasks[4].pin, task_handle4, CHANGE);
  attachInterrupt(tasks[5].pin, task_handle5, CHANGE);
}

/**
 * Initialize buttons with callback functions
 */
static void button_init()
{
  ProductoButton *buttons = producto.buttons;
  
  buttons[0].btn.setTapHandler(toggle_list_tasks);
  buttons[1].btn.setTapHandler(pause_resume_timer);  
  
  attachInterrupt(buttons[0].pin, button_handle0, CHANGE);
  attachInterrupt(buttons[1].pin, button_handle1, CHANGE);
}

static void wifi_init()
{
  for (int i = 0; i < NUM_WIFI; i++)
    {
      wifiMulti.addAP(ssid[i], password[i]);
    }
  
  Serial.println("Connecting Wifi...");
  for (int i = 0; wifiMulti.run() != WL_CONNECTED && i < 40; i++)
    {
      delay(250);
      Serial.print('.');
    }

  if (wifiMulti.run() == WL_CONNECTED) {
    Serial.println('\n');
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());

    // if (MDNS.begin("esp32")) {
    //   Serial.println("mDNS responder started");
    // } else {
    //   Serial.println("Error setting up MDNS responder!");
    // }
  } else {
    Serial.println("Timeout connecting to Wifi, will operate on defaults.");
  }
}

////////////////////////////////////////////////////////////////////////////////

static void task_btn(int task_num)
{
  /* TODO: really this should be cooked into the Display FSM to transition stately */
  DisplayState display_state = display_get_state();
  
  if ( task_num != producto.active_task
       || display_state != TASK /* TODO: display state hack */
       || producto.active_task == PRODUCTO_TASK_NONE ) {
    producto.active_task = task_num;
    producto.paused_task = PRODUCTO_TASK_NONE;
    display_transition(TASK);
  }
}

static void toggle_list_tasks(Button2&)
{
  /* TODO: really this should be cooked into the Display FSM to transition stately */
  DisplayState display_state = display_get_state();

  if (display_state == LIST && (producto.active_task != PRODUCTO_TASK_NONE || producto.paused_task != PRODUCTO_TASK_NONE)) {
    display_transition(TASK);
  } else if (display_state == TASK) {
    display_transition(LIST);
  } else {
    // Do nothing
  }
}

////////////////////////////////////////////////////////////////////////////////

static void save_active_state()
{
  String producto_str = "{ ";

  producto_str += "\"start_time\" : ";
  producto_str += producto.start_time.unixtime();
  producto_str += ", ";
  

  for (int i = 0; i < producto.num_tasks; i++) {
    producto_str += "\"";
    producto_str += producto.tasks[i].str;
    producto_str += "\"";
    producto_str += " : ";
    producto_str += producto.tasks[i].timer;
    producto_str += ", ";
  }
  
  producto_str += "}\r\n";

  producto_str.toCharArray(very_big_arr, VERY_BIG);
  myspiffs_write_file(PRODUCTO_ACTIVE_FILE, very_big_arr, true);
}

static void append_active_to_tasks_file()
{
  String active_str = myspiffs_read_first_line_of_file(PRODUCTO_ACTIVE_FILE);
  active_str += "\r\n";
  active_str.toCharArray(very_big_arr, VERY_BIG);
  myspiffs_append_file(PRODUCTO_TASK_FILE, very_big_arr);
}

static String strip(String str)
{
  String out_str = str;
  
  if (out_str[out_str.length() - 1] == '\n') {
    out_str = out_str.substring(0,out_str.length()-1);
  }
    
  if (out_str[out_str.length() - 1] == '\r') {
    out_str = out_str.substring(0,out_str.length()-1);
  }

  return out_str;
}

static void handle_serial_commands()
{
  String str, int_str, task_str = "";
  int task_num;

  if (Serial.available() > 0) {
    str = Serial.readString();
    str = strip(str);
    
    if (str == "read") {
      DEBUG_PRINTLN("READ");
      print_task_history();
    }

    else if (str == "delete") {
      DEBUG_PRINTLN("DELETE");
      delete_task_history();
    }

    else if (str.startsWith("task")) {
      // Parse out task number and string
      str = str.substring(5);                         // ignore "task "
      int_str = str.substring(0, str.indexOf(' '));   // read the task number
      task_num = str.toInt();                         // convert to a string
      
      if (str.indexOf(' ') > 0) {
	task_str = str.substring(str.indexOf(' ') + 1); // read the task string
      }
      
      // Print em out
      DEBUG_PRINTF("Requesting rename of task #%d to %s\r\n", task_num, task_str);
      rename_task(producto, task_num, task_str);
    }
  }
}
