#define CTRL_KEY(k)

/* clears terminal screen
   https://en.wikipedia.org/wiki/VT100
   https://vt100.net/docs/vt100-ug/chapter3.html#ED
*/
void fullClearTerminal();
void clearTerminal();


void die(const char *s);
int getWindowSize(int *rows, int *cols);


/* RAW MODE */

void disableRawMode();
void enableRawMode();


/* EDITOR */

void initEditor();
char editorReadKey();
void editorProcessKeypress();
void editorDrawRows();
