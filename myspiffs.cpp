#include "Arduino.h"
#include "SPIFFS.h"
#include "myspiffs.h"

void myspiffs_init()
{
  if(!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
  }
}

void myspiffs_list_dir(const char * dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = SPIFFS.open(dirname);
  if(!root){
    Serial.println("- failed to open directory");
    return;
  }
  if(!root.isDirectory()){
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if(levels){
	myspiffs_list_dir(file.name(), levels -1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void myspiffs_read_file(const char * path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = SPIFFS.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while(file.available()){
    Serial.write(file.read());
  }
}

void myspiffs_write_file(const char * path, const char * message, bool line)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = SPIFFS.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }

  if (line) {
    if(file.println(message)){
      Serial.println("- file written");
    } else {
      Serial.println("- frite failed");
    }
  } else {
    if(file.print(message)){
      Serial.println("- file written");
    } else {
      Serial.println("- frite failed");
    }
  }
}

void myspiffs_append_file(const char * path, const char * message, bool line)
{
  Serial.printf("Appending to file: %s\r\n", path);

  File file = SPIFFS.open(path, FILE_APPEND);
  if(!file){
    Serial.println("- failed to open file for appending");
    return;
  }

  if (line) {
    if(file.println(message)){
      Serial.println("- file written");
    } else {
      Serial.println("- frite failed");
    }
  } else {
    if(file.print(message)){
      Serial.println("- file written");
    } else {
      Serial.println("- frite failed");
    }
  }
}

void myspiffs_rename_file(const char * path1, const char * path2)
{
  Serial.printf("Renaming file %s to %s\r\n", path1, path2);
  if (SPIFFS.rename(path1, path2)) {
    Serial.println("- file renamed");
  } else {
    Serial.println("- rename failed");
  }
}

void myspiffs_delete_file(const char * path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if(SPIFFS.remove(path)){
    Serial.println("- file deleted");
  } else {
    Serial.println("- delete failed");
  }
}
