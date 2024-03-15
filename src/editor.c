#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include "headers/buf.h"


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

void editorDrawRows(struct abuf *ab) {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        abAppend(ab, "~", 1);
        if (y < E.screenrows - 1) {
            abAppend(ab, "\r\n", 2);
        }
    }
}


// clears terminal screen
// https://en.wikipedia.org/wiki/VT100
// https://vt100.net/docs/vt100-ug/chapter3.html#ED
void fullClearTerminal(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

void editorRefreshScreen() {
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\x1b[2J", 4);
    abAppend(&ab, "\x1b[H", 3);
    editorDrawRows(&ab);
    abAppend(&ab, "\x1b[H", 3);
    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void die(const char *s) {
    fullClearTerminal();
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


int getCursorPosition(int *rows, int *cols) {
      char buf[32];
    unsigned int i = 0;
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    printf("\r\n&buf[1]: '%s'\r\n", &buf[1]);
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
    return 0;
}


int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}