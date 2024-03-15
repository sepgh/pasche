#include "headers/editor.h"


int main() {
    enableRawMode();
    initEditor();
    while (1) {
        clearTerminal();
        editorProcessKeypress();
    }
    
    return 0;
}