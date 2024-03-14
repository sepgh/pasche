#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include "headers/terminal.h"
#include <errno.h>


int main() {
    enableRawMode();

    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) 
            die("read");
        
        if (c == 0) continue;
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == CTRL_KEY('q')) break;
    }
    
    disableRawMode();
    return 0;
}