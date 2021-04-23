#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    window.cursor_pos = 0;
    
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
    XDrawString(window.dis, window.win, window.gc, 0, window.cursor_pos + 10, str, len);
    window.cursor_pos += 12;
}

void init_term() {
    term.buf = malloc(sizeof(char) * X_BUF_SIZ);
    term.buf_pos = 0;
    term.buf_size = X_BUF_SIZ;
}

void close_term() {
    free(term.buf);
}

void term_read() {
    int read;

    if ((read = shell_read(&term.buf[term.buf_pos], term.buf_size - term.buf_pos - 1)) >= 0) {
	term.buf_pos += read;
	if (term.buf_pos + 1 >= term.buf_size) {
	    term.buf = realloc(term.buf, term.buf_size * 2);
	    term.buf_size *=2;
	}
	send_redraw_event();
    }
}

void term_clear_buf() {
    term.buf_pos = 0;
}

int x_handle_event(XEvent *ev) {
    XNextEvent(window.dis, ev);

    switch(ev->type) {
    case Expose:
	x_draw_string(term.buf, term.buf_pos);
	term_clear_buf();
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
	    loop |= x_handle_event(&ev);
	}
	
	term_read();
    }
}
