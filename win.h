#ifndef __WIN_H__
#define __WIN_H__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xft/Xft.h>


#include "ansi.h"
#include "font.h"

#define MARGIN_X 10
#define MARGIN_Y 20

struct TermWindow {
    Display *dis;
    int screen;
    Window win;
    GC gc;
    unsigned long black, white;
    unsigned int width, height;
    XftFont* font;
    XftDraw* draw;
    int pos_x;
    int pos_y;
};

int init_x_win();
void close_x_win();
void send_redraw_event();
void x_draw_string(char* str, unsigned int len);
void render_glyphs(struct FtGlyph* glyphs, int len);
void expose();
void draw_sequence(struct EscapeSequence* seq);


struct HistoryLine {
    struct EscapeSequence* sequences;
    int len;
};

#define X_BUF_SIZ 1024
#define LINE_HISTORY 1024
#define SEQUENCES_PER_LINE 1024

struct Term {
    char* buf;
    int buf_pos;
    unsigned int buf_size;
    int current_col;
    int current_line;
    int current_scroll;
    struct HistoryLine history[LINE_HISTORY];
    int history_write_current_line;
    int history_read_current_line;
    int history_read_pos;
    int history_full;
};

void init_term();
void close_term();
void term_read_shell();
void term_clear_buf();

void main_event_loop();

#endif
