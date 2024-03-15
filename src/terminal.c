#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>


/* --- DATA --- */
struct editorConfig {
  struct termios orig_termios;
};

struct editorConfig E;


void editorDrawRows() {
    int y;
    for (y = 0; y < 24; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}


// clears terminal screen
// https://en.wikipedia.org/wiki/VT100
// https://vt100.net/docs/vt100-ug/chapter3.html#ED
void fullClearTerminal(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void clearTerminal() {
    fullClearTerminal();
    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void die(const char *s) {
    clearTerminal();
    perror(s);
    exit(1);
}


/* RAW MODE */

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = E.orig_termios;

    // Input output flags
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Input timeouts
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}


