#include <pthread.h>

#include "globals.h"
#include "terminal.h"
#include "utils.h"
#include "editor.h"

extern struct editorConfig E;
extern struct programUtils PU;
extern struct key KEY;
extern struct keyValue V;
extern struct debugUtil DEB;
extern textbuf TEXTBUF;

/*** init ***/
void init(void) {
  E.cx = 0; // E is global variable
  E.cy = 0;
  E.offsety = 0;
  E.offsetx = 0;
	E.mode = 1; // 1 insert mode
  E.leftMarginSize = 3;
  getWindowSize(&E.screenrows, &E.screencols); // from "terminal.h"
	
	programUtilsInit(&PU);
	textbufInit(&TEXTBUF);
	keyInit(&KEY);
	keyValueInit(&V);
	debugUtilInit(&DEB);

  enableRAWMode(); // from "terminal.h"; enable Terminal RAW mode
}

int main(int argc, char *argv[]) {
  init();
  if (argc > 1) {
    editorOpen(argv[1]);
  } 
  if (argc <= 1) {
    editorOpen("aaa.txt");
  }
  // Margin Size depends on the textbuffer read.
  editorSetMarginSize(&E, &TEXTBUF);
  E.cx = E.leftMarginSize;
  while (PU.running) { // PU is global struct, [P]rogram [U]tils
    if (editorReadKey() == -1)
      die("editorReadKey Failed");
    if (PU.updated) {
      if (KEY.key[0]){
        // Incase the margine size changes
        editorSetMarginSize(&E, &TEXTBUF);
        editorProcessKeyPress();
        keyRefresh(&KEY);
      }
      editorRefreshScreen();
      PU.updated = 0;
    }
  }

  if (argc > 1) {
    editorSaveFile(argv[1]);
  } 
	if (argc <= 1) {
		editorSaveFile("savedTo.txt");
	}
	clearScreen();
  return 0;
} 
