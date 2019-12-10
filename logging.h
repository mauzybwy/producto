#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOGGING_ERR 0
#define LOGGING_DBG 1
#define LOGGING_ALL 2

#define LOG_LEVEL LOGGING_ERR

#if LOG_LEVEL >= LOGGING_ERR
#define ERR_PRINT(x) Serial.print(x);
#define ERR_PRINTLN(x) Serial.println(x);
#define ERR_PRINTF(x) Serial.printf(x);
#else
#define ERR_PRINT(x)
#define ERR_PRINTLN(x)
#define ERR_PRINTF(x)
#endif

#if LOG_LEVEL >= LOGGING_DBG
#define DEBUG_PRINT(x) Serial.print(x);
#define DEBUG_PRINTLN(x) Serial.println(x);
#define DEBUG_PRINTF(x) Serial.printf(x);
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x)
#endif

#endif // _LOGGING_H_
