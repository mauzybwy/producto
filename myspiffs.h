#ifndef _MYSPIFFS_H_
#define _MYSPIFFS_H_

#include "Arduino.h"

void myspiffs_init();
void myspiffs_list_dir(const char * dirname, uint8_t levels);
void myspiffs_read_file(const char * path);
void myspiffs_write_file(const char * path, const char * message, bool line = false);
void myspiffs_append_file(const char * path, const char * message, bool line = false);
void myspiffs_rename_file(const char * path1, const char * path2);
void myspiffs_delete_file(const char * path);

#endif // _MYSPIFFS_H_
