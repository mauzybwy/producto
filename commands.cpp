////////////////////////////////////////////////////////////////////////////////

#include "commands.h"
#include "producto.h"
#include "logging.h"
#include "myspiffs.h"
#include "shared_mem.h"

#include "Arduino.h"

////////////////////////////////////////////////////////////////////////////////

static void print_task_file_contents();
static void print_active_file_contents();

////////////////////////////////////////////////////////////////////////////////

void print_task_history()
{
  print_task_file_contents();
  print_active_file_contents();
}

void delete_task_history()
{
  myspiffs_delete_file(PRODUCTO_TASK_FILE);
}

void rename_task(Producto producto, int task_num, String task_str)
{
  String path = "/task/", fname;
  
  // Valid task numbers are 1 -> PRODUCTO_TASKS
  if (task_num > 0 && task_num <= producto.num_tasks && task_str.length() > 0) {
    DEBUG_PRINTF("Renaming task #%d to %s\r\n", task_num, task_str);

    fname = path + producto.tasks[task_num-1].id;
    fname.toCharArray(big_arr, BIG);
    task_str.toCharArray(very_big_arr, VERY_BIG);
	
    myspiffs_write_file(big_arr, very_big_arr, true);
    producto.tasks[task_num-1].str = String(task_str);
  }
}

void update_current_time()
{
  
}

String get_current_task(Producto *producto)
{
  String retVal;
  
  if (producto->active_task != PRODUCTO_TASK_NONE) {
    retVal = producto->tasks[producto->active_task].str;
  } else if(producto->paused_task != PRODUCTO_TASK_NONE) {
    retVal = producto->tasks[producto->paused_task].str;
  } else {
    retVal = "No active tasks";
  }

  return retVal;
}

String get_task_file_contents()
{
  return myspiffs_read_file(PRODUCTO_TASK_FILE);
}

String get_active_file_contents()
{
  return myspiffs_read_first_line_of_file(PRODUCTO_ACTIVE_FILE);
}

////////////////////////////////////////////////////////////////////////////////

static void print_task_file_contents()
{
  myspiffs_print_file_to_serial(PRODUCTO_TASK_FILE);
}

static void print_active_file_contents()
{
  myspiffs_print_file_to_serial(PRODUCTO_ACTIVE_FILE);
}

