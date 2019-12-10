#include "Arduino.h"
#include "SPIFFS.h"
#include "myspiffs.h"
#include "logging.h"

void myspiffs_init()
{
  if(!SPIFFS.begin()) {
    ERR_PRINTLN("SPIFFS Mount Failed")
  }
}

void myspiffs_list_dir(const char * dirname, uint8_t levels)
{
  DEBUG_PRINTF(("Listing directory: %s\r\n", dirname))

  File root = SPIFFS.open(dirname);
  if(!root){
    ERR_PRINTLN("- failed to open directory")
    return;
  }
  if(!root.isDirectory()){
    ERR_PRINTLN(" - not a directory")
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      DEBUG_PRINT("  DIR : ")
      DEBUG_PRINTLN(file.name())
      if(levels){
	myspiffs_list_dir(file.name(), levels -1);
      }
    } else {
      DEBUG_PRINT("  FILE: ")
      DEBUG_PRINT(file.name())
      DEBUG_PRINT("\tSIZE: ")
      DEBUG_PRINTLN(file.size())
    }
    file = root.openNextFile();
  }
}

void myspiffs_print_file_to_serial(const char * path)
{
  DEBUG_PRINTF(("Reading file: %s\r\n", path))

  File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
    ERR_PRINTLN("- failed to open file for reading")
    return;
  }

  LOG_PRINTLN("- read from file:")
  while(file.available()){
    Serial.write(file.read());
  }
}

String myspiffs_read_first_line_of_file(const char * path)
{
  LOG_PRINTF(("Reading first line of file: %s\r\n", path))

  File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
    LOG_PRINTLN("- failed to open file for reading")
    return "";
  }

  String first_line = "";
  char c;
  while(file.available()) {
    c = (char)file.read();
    if (c == '\r' || c == '\n') {
      break;
    }
    
    first_line += c;
  }
  LOG_PRINTLN("- first line:")
  LOG_PRINTLN(first_line)
  return first_line;
}

void myspiffs_write_file(const char * path, const char * message, bool line)
{
  LOG_PRINTF(("Writing file: %s\r\n", path))
  
  File file = SPIFFS.open(path, FILE_WRITE);
  if(!file){
    LOG_PRINTLN("- failed to open file for writing")
    return;
  }

  if (line) {
    if(file.println(message)){
      LOG_PRINTLN("- file written")
    } else {
      LOG_PRINTLN("- frite failed")
    }
  } else {
    if(file.print(message)){
      LOG_PRINTLN("- file written")
    } else {
      LOG_PRINTLN("- frite failed")
    }
  }
}

void myspiffs_append_file(const char * path, const char * message, bool line)
{
  LOG_PRINTF(("Appending to file: %s\r\n", path))

  File file = SPIFFS.open(path, FILE_APPEND);
  if(!file){
    LOG_PRINTLN("- failed to open file for appending")
    return;
  }

  if (line) {
    if(file.println(message)){
      LOG_PRINTLN("- file written")
    } else {
      LOG_PRINTLN("- frite failed")
    }
  } else {
    if(file.print(message)){
      LOG_PRINTLN("- file written")
    } else {
      LOG_PRINTLN("- frite failed")
    }
  }
}

void myspiffs_rename_file(const char * path1, const char * path2)
{
  LOG_PRINTF(("Renaming file %s to %s\r\n", path1, path2))
  if (SPIFFS.rename(path1, path2)) {
    LOG_PRINTLN("- file renamed")
  } else {
    LOG_PRINTLN("- rename failed")
  }
}

void myspiffs_delete_file(const char * path)
{
  LOG_PRINTF(("Deleting file: %s\r\n", path))
  if(SPIFFS.remove(path)){
    LOG_PRINTLN("- file deleted")
  } else {
    LOG_PRINTLN("- delete failed")
  }
}
