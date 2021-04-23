#include <stdio.h>
#include "win.h"
#include "sh.h"

int main(int argc, char** argv) {
    init_x_win();
    init_term();
    open_shell();
    main_event_loop();
    close_x_win();
    return 0;
}
