#include <err.h>
#include <errno.h>
#include <poll.h>
#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>

#include "sh.h"
#include "util.h"

struct Shell sh;

void open_shell() {
    if ((sh.pid = forkpty(&sh.master_fd, NULL, NULL, NULL)) == 0) {
	char* args[2];
	args[0] = "/bin/fish";
	args[1] = NULL;
	if (execvp("/bin/fish", args) < 0) {
	    // TODO: close main term window with error
	    exit(1);
	}
    }
}

int shell_read(char *buf, unsigned int len) {
    struct pollfd fd;
    fd.fd = sh.master_fd;
    fd.events = POLLIN;

    int res_poll;
    int pos = 0;
    
    while((res_poll = poll(&fd, 1, 16)) >= 0) {
	if (fd.revents & POLLIN) {
	    int read_bytes;
	    if ((read_bytes = read(sh.master_fd, &buf[pos], len - pos)) < 0) {
		error_exit("shell_read: read");
	    }
	    pos += read_bytes;

	    if (pos +1 >= len) {
		return pos;
	    }
	} else {
	    break;
	}
    }
    
    if (res_poll < 0) {
	error_exit("poll");
    }

    return pos;
}
