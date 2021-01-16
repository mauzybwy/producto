#ifndef _MYSPIFFS_H_
#define _MYSPIFFS_H_

#include "Arduino.h"

void myspiffs_init();
bool myspiffs_file_exists(const char * path);
bool myspiffs_directory_exists(const char * path);
void myspiffs_list_dir(const char * dirname, uint8_t levels);
void myspiffs_print_file_to_serial(const char * path);
String myspiffs_read_first_line_of_file(const char * path);
String myspiffs_read_file(const char * path);
void myspiffs_write_file(const char * path, const char * message, bool line = false);
void myspiffs_append_file(const char * path, const char * message, bool line = false);
void myspiffs_rename_file(const char * path1, const char * path2);
void myspiffs_delete_file(const char * path);

#endif // _MYSPIFFS_H_
