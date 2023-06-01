#include <pthread.h>

#include "globals.h"
#include "terminal.h"
#include "utils.h"
#include "editor.h"

extern struct editorConfig E;
extern struct programUtils PU;
extern struct key KEY;
extern struct keyValue V;
extern textbuf TEXTBUF;

/*** init ***/
void init(void) {
  E.cx = 0; // E is global variable
  E.cy = 0;
  E.offsety = 0;
  E.offsetx = 0;
	E.mode = 1; // 1 insert mode
  getWindowSize(&E.screenrows, &E.screencols); // from "terminal.h"
	
	programUtilsInit(&PU);
	textbufInit(&TEXTBUF);
	keyInit(&KEY);
	keyValueInit(&V);

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
  while (PU.running) { // PU is global struct, [P]rogram [U]tils
		if (editorReadKey() == -1)
			die("editorReadKey Failed");
    if (PU.updated) {
			if (KEY.key[0]){
				editorProcessKeyPress();
				keyRefresh(&KEY);
			}
      editorRefreshScreen();
      PU.updated = 0;
    }
  }
	clearScreen();
  return 0;
} 
