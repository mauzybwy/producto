#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "producto.h"
#include "Arduino.h"

void print_task_history();
void delete_task_history();
void rename_task(Producto producto, int task_num, String task_str);
void update_current_time();
String get_current_task(Producto *producto);

String get_task_file_contents();
String get_active_file_contents();

#endif // _COMMANDS_H_
