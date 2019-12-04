#ifndef _LOGGING_H_
#define _LOGGING_H_

#define LOGGING_DISABLED false
#define LOGGING_ENABLED true

#define LOG_LEVEL LOGGING_DISABLED

#if LOG_LEVEL == LOGGING_DISABLED
#define LOG_PRINT(x)
#define LOG_PRINTLN(x)
#define LOG_PRINTF(x)
#else
#define LOG_PRINT(x) Serial.print(x);
#define LOG_PRINTLN(x) Serial.println(x);
#define LOG_PRINTF(x) Serial.printf(x);
#endif

#endif // _LOGGING_H_
