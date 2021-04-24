#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ansi.h"
#include "font.h"
#include "util.h"
#include "win.h"
#include "sh.h"

struct TermWindow window;
struct Term term;

int init_x_win() {
    Display *dis;
    int screen;
    Window win;
    GC gc;
    unsigned long black, white;

    dis = XOpenDisplay((char *) 0);
    screen = DefaultScreen(dis);
    black = BlackPixel(dis, screen);
    white = WhitePixel(dis, screen);
    unsigned int width = 200;
    unsigned int height = 200;

    win = XCreateSimpleWindow(dis, DefaultRootWindow(dis), 0, 0, width, height, 0, black, white);

    XSetStandardProperties(dis, win, "ft", "", None, NULL, 0, NULL);

    XSelectInput(dis, win, ExposureMask | ButtonPressMask | KeyPressMask);

    gc = XCreateGC(dis, win, 0, 0);

    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);

    XClearWindow(dis, win);
    XMapWindow(dis, win);
    
    window.dis = dis;
    window.screen = screen;
    window.win = win;
    window.gc = gc;
    window.black = black;
    window.white = white;
    window.width = width;
    window.height = height;
    window.pos_x = MARGIN_X;
    window.pos_y = MARGIN_Y;
    
    window.font = XftFontOpenName(window.dis, screen, "Iosevka");
    window.draw = XftDrawCreate(window.dis, window.win, DefaultVisual(window.dis, screen), DefaultColormap(window.dis, screen));

    load_font(window.font);
 
    return 0;
}

void close_x_win() {
    XFreeGC(window.dis, window.gc);
    XDestroyWindow(window.dis, window.win);
    XCloseDisplay(window.dis);
}

void send_redraw_event() {
    XEvent expose_event;
    expose_event.type = Expose;
    expose_event.xexpose.window = window.win;
    XSendEvent(window.dis, window.win, False, ExposureMask, &expose_event);
}

void x_draw_string(char *str, unsigned int len) {
    DEBUG_LOG("Drawing %.*s\n", len, str);
    struct FtGlyph* glyphs = malloc(sizeof(struct FtGlyph) * 4096);
    int glyph_count = get_glyphs_shaped(str, len, glyphs);
    render_glyphs(glyphs, glyph_count);
    free(glyphs);
}

void render_glyphs(struct FtGlyph *glyphs, int len) {
    uint32_t* points = malloc(sizeof(uint32_t) * len);
    XRenderColor color = {0, 0, 0, 0xFFFF};
    XftColor xftc;
    XftColorAllocValue(window.dis, DefaultVisual(window.dis, window.screen), DefaultColormap(window.dis, window.screen), &color, &xftc);

    int new_pos_x = window.pos_x;
    int new_pos_y = window.pos_y;
    
    for (int i = 0; i < len; i++) {
	points[i] = glyphs[i].glyph_index;
	new_pos_x += glyphs[i].x_advance;
	new_pos_y += glyphs[i].y_advance;
    }
    
    XftDrawGlyphs(window.draw, &xftc, window.font, window.pos_x + glyphs[0].x_offset, window.pos_y + glyphs[0].y_offset, points, len);

    window.pos_x = new_pos_x;
    window.pos_y = new_pos_y;

    DEBUG_LOG("New pos_x: %d\n", window.pos_x);
    
    free(points);
}

void expose() {
    struct HistoryLine line;
    struct EscapeSequence* seq;

    int i;
    int s;
    for (i = term.history_read_current_line; i <= term.history_write_current_line; i++) {
	line = term.history[i];
	for (s = term.history_read_pos; s < line.len; s++) {
	    seq = &line.sequences[s];

	    draw_sequence(seq);
	}
    }

    term.history_read_current_line = term.history_write_current_line;
    term.history_read_pos = line.len;
}

void draw_sequence(struct EscapeSequence *seq) {
    
    switch (seq->seq) {
    case LiteralString:
	x_draw_string(seq->literal_str, seq->literal_str_len);
	break;
    case SingleCharacterSequence:
	if (seq->single_character_seq == ANSI_LF) {
	    window.pos_y += 20;
	} else if (seq->single_character_seq == ANSI_CR) {
	    window.pos_x = MARGIN_X;
	}
    default:
	//DEBUG_LOG("Unhandled sequence while drawing: %d\n", seq->seq);
	break;
    }
}

void init_term() {
    term.buf = malloc(sizeof(char) * X_BUF_SIZ);
    term.buf_pos = 0;
    term.buf_size = X_BUF_SIZ;

    term.current_col = 0;
    term.current_line = 0;
    term.current_scroll = 0;

    for (int i = 0; i < LINE_HISTORY; i++) {
	term.history[i].sequences = malloc(sizeof(struct EscapeSequence) * SEQUENCES_PER_LINE);
    }

    term.history_write_current_line = 0;
    term.history_full = 0;
}

void close_term() {
    free(term.buf);
    for (int i = 0; i < LINE_HISTORY; i++) {
	free(term.history[i].sequences);
    }
}

void term_read_shell() {
    int read;
    int str_pos = term.buf_pos;

    if ((read = shell_read(&term.buf[term.buf_pos], term.buf_size - term.buf_pos - 1)) >= 0) {
	term.buf_pos += read;
	if (term.buf_pos + 1 >= term.buf_size) {
	    term.buf = realloc(term.buf, term.buf_size * 2);
	    term.buf_size *=2;
	}
    } else {
	return;
    }

    while (str_pos < term.buf_pos) {
	struct HistoryLine* line = &term.history[term.history_write_current_line];
	struct EscapeSequence* seq = &line->sequences[line->len];
	
	str_pos += read_ansi_sequence(&term.buf[str_pos], term.buf_size - str_pos, seq);

	// Merging two literal strings if that happens
	if (line->len > 0) {
	    struct EscapeSequence* previous_seq = &line->sequences[line->len - 1];
	    if (previous_seq->seq == LiteralString && seq->seq == LiteralString) {
		if (previous_seq->literal_str_len + seq->literal_str_len < SEQUENCE_LITERAL_STR_LEN) {
		    memcpy(&previous_seq->literal_str_buffer[previous_seq->literal_str_len], seq->literal_str_buffer, seq->literal_str_len);
		    previous_seq->literal_str_len += seq->literal_str_len;
		    line->len--;
		}
	    }
	}
	
	line->len++;

	
	if (seq->seq == SingleCharacterSequence)  {
	    if (seq->single_character_seq == ANSI_LF) {
		term.history_write_current_line ++;
		term.history[term.history_write_current_line].len = 0;
	    }
	}
    }

    send_redraw_event();
}

void term_clear_buf() {
    term.buf_pos = 0;
}

int x_handle_event(XEvent *ev) {
    XNextEvent(window.dis, ev);

    switch(ev->type) {
    case Expose:
	expose();
	break;

    case KeyPress:
	break;
    default:
	break;
    }

    return 1;
}

void main_event_loop() {
    XEvent ev;
    int pos = 0;
    
    int loop = 1;
    
    while(loop) {
	
	while(XPending(window.dis)) {
	    loop &= x_handle_event(&ev);
	}
	
	term_read_shell();
    }
}
