#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define LOG_INFO(...) { printf( __VA_ARGS__  ); }

#define LOG_DEBUG(...) { /*nope*/ }
/*
 * enable this, if you also want to output the log level debug
 #define LOG_DEBUG(...) { printf( __VA_ARGS__  ); }
*/

#define LOG_ERROR(...) { printf( __VA_ARGS__  ); }

#endif
