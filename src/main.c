#include "globals.h"
#include "terminal.h"
#include "utils.h"
#include "editor.h"

extern struct editorConfig E;
extern struct programUtils PU;
extern struct key KEY;
extern textbuf TEXTBUF;

/*** init ***/
void init(void) {
  E.cx = 0; // E is global variable
  E.cy = 0;
  E.offsety = 0;
  E.offsetx = 0;
  getWindowSize(&E.screenrows, &E.screencols); // from "terminal.h"
	
	programUtilsInit(&PU);
	textbufInit(&TEXTBUF);
	keyInit(&KEY);
}

int main(int argc, char *argv[]) {
  enableRAWMode(); // from "terminal.h"; enable Terminal RAW mode
  init();
  if (argc > 1) {
    editorOpen(argv[1]);
  }
  while (PU.running) { // PU is global struct, [P]rogram [U]tils
    if (PU.updated) {
      editorRefreshScreen();
      PU.updated = 0;
    }
    editorProcessKeyPress();
  }
  return 0;
}
