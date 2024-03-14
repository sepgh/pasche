#include "headers/io.h"
#include "headers/terminal.h"


int main() {
    enableRawMode();

    while (1) {
        clearTerminal();
        editorProcessKeypress();
    }
    
    return 0;
}