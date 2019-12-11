#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOGGING_NONE 0
#define LOGGING_ERR 1
#define LOGGING_DBG 2
#define LOGGING_ALL 3

#define LOG_LEVEL LOGGING_ERR

#if LOG_LEVEL >= LOGGING_ERR
#define ERR_PRINT(x) Serial.print(x);
#define ERR_PRINTLN(x) Serial.println(x);
#define ERR_PRINTF(...) Serial.printf(__VA_ARGS__);
#else
#define ERR_PRINT(x)
#define ERR_PRINTLN(x)
#define ERR_PRINTF(...)
#endif

#if LOG_LEVEL >= LOGGING_DBG
#define DEBUG_PRINT(x) Serial.print(x);
#define DEBUG_PRINTLN(x) Serial.println(x);
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__);
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(...)
#endif

#endif // _LOGGING_H_
