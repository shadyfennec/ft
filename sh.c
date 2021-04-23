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


struct Shell sh;

void open_shell() {
    if ((sh.pid = forkpty(&sh.master_fd, NULL, NULL, NULL)) == 0) {
	char* args[2];
	args[0] = "/bin/fish";
	args[1] = NULL;
	if (execvp("/bin/fish", args) < 0) {
	    FILE *f = fopen("log", "w");
	    fprintf(f, "%s", strerror(errno));
	    exit(1);
	}
    }
}

int shell_read(char *buf, unsigned int len) {
    struct pollfd fd;
    fd.fd = sh.master_fd;
    fd.events = POLLIN;

    if (poll(&fd, 1, 0) < 0) {
	perror("poll");
	exit(1);
    }

    
    if (fd.revents & POLLIN) {
	int read_bytes = -1;
	if ((read_bytes = read(sh.master_fd, buf, len)) < 0) {
	    perror("shell_read: read");
	    exit(1);
	}
	return read_bytes;
    }

    return -1;
}
