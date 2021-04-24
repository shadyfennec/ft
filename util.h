#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#define DEBUG_LOG(...) fprintf(stderr, "[DEBUG] "); fprintf(stderr, __VA_ARGS__)

void error_exit(const char* location);
void print_char_array(char* array, int len);
int get_utf_8_len(unsigned char c);
int min(int a, int b);
int max(int a, int b);

#endif
