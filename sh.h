#ifndef __SH_H__
#define __SH_H__

#include <unistd.h>

struct Shell {
    pid_t pid;
    int master_fd;
};

void open_shell();
int shell_read(char *buf, unsigned int len);

#endif
