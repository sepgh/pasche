/* Header file for src/terminal.c */

void die(const char *s);
void enableRawMode();
void disableRawMode();


/* Definitions */

/*
The CTRL_KEY macro bitwise-ANDs a character with the value 00011111, in binary
In other words, it sets the upper 3 bits of the character to 0.
This mirrors what the Ctrl key does in the terminal: it strips bits 5 and 6 from whatever key you press in combination with Ctrl, and sends that.

https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html#press-ctrl-q-to-quit
*/
#define CTRL_KEY(k) ((k) & 0x1f)
