#ifndef __WIN_H__
#define __WIN_H__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#define X_BUF_SIZ 1024

struct TermWindow {
    Display *dis;
    int screen;
    Window win;
    GC gc;
    unsigned long black, white;
    unsigned int width, height;
    unsigned int cursor_pos;
};

int init_x_win();
void close_x_win();
void send_redraw_event();
void x_draw_string(char* str, unsigned int len);

struct Term {
    char* buf;
    int buf_pos;
    unsigned int buf_size;
};

void init_term();
void close_term();
void term_read();
void term_clear_buf();

void main_event_loop();

#endif
