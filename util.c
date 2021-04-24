#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void error_exit(const char *location) {
    perror(location);
    exit(1);
}

void print_char_array(char *array, int len) {
    fprintf(stderr, "[");
    for (int i = 0; i <  len; i++) {
	fprintf(stderr, "0x%02x ", (unsigned char)array[i]);
    }
    fprintf(stderr, "]\n");
}

int get_utf_8_len(unsigned char c) {
    if ((c >> 7) == 0x0) return 1;
    if ((c >> 5) == 0x6) return 2;
    if ((c >> 4) == 0xe) return 3;
    if ((c >> 3) == 0x1e) return 4;
    return -1;
}

int min(int a, int b) { return a > b ? b : a; }

int max(int a, int b) { return a > b ? a : b; }
