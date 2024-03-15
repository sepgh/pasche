#include "buf.h"
/* Definitions */

/*
The CTRL_KEY macro bitwise-ANDs a character with the value 00011111, in binary
In other words, it sets the upper 3 bits of the character to 0.
This mirrors what the Ctrl key does in the terminal: it strips bits 5 and 6 from whatever key you press in combination with Ctrl, and sends that.

https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#press-ctrl-q-to-quit
*/
#define CTRL_KEY(k) ((k) & 0x1f)
#define PASCHE_VERSION "0.0.1"

/* EDITOR */

void editorDrawRows(struct abuf *ab);

// clears terminal screen
// https://en.wikipedia.org/wiki/VT100
// https://vt100.net/docs/vt100-ug/chapter3.html#ED
void fullClearTerminal();
void editorRefreshScreen();
void die(const char *s);

/* RAW MODE */

void disableRawMode();
void enableRawMode();

/* EDITING */

void editorMoveCursor(int key);
int editorReadKey();
void editorProcessKeypress();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);
void initEditor();