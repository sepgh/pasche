#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>


/* Definitions */

/*
The CTRL_KEY macro bitwise-ANDs a character with the value 00011111, in binary
In other words, it sets the upper 3 bits of the character to 0.
This mirrors what the Ctrl key does in the terminal: it strips bits 5 and 6 from whatever key you press in combination with Ctrl, and sends that.

https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#press-ctrl-q-to-quit
*/
#define CTRL_KEY(k) ((k) & 0x1f)


/* --- DATA --- */
struct editorConfig {
    struct termios orig_termios;
    int screenrows;
    int screencols;
};

struct editorConfig E;




/* --- API --- */

void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
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


int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}


char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}


void editorProcessKeypress() {
    char c = editorReadKey();
    switch (c) {
        case CTRL_KEY('q'):
            fullClearTerminal();
            exit(0);
            break;
    }
}