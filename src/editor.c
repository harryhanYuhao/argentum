#include "utils.h"
#include "globals.h"
#include "editor.h"

extern struct editorConfig E;
extern struct programUtils PU;
extern struct key KEY;
extern struct keyValue V;
extern struct debugUtil DEB;
extern textbuf TEXTBUF;

/*** FILE IO ***/
// Change to open() syscall
void editorOpen(const char *filename) {
  FILE *fp = fopen(filename, "ab+");
  if (!fp) {
    char errorMessage[100];
    snprintf(errorMessage, 100, "Can not open File '%s'\r\nperrer message",
             filename);
    die(errorMessage);
  }

  textbufRead(&TEXTBUF, fp); // All lines of the file are read into TEXTBUF
  fclose(fp);
	if (TEXTBUF.size==0){
		textbufInitForEmptyFile(&TEXTBUF);
	}
}

/*** Input ***/
/// Reads and returns the key once.
int editorReadKey(void) {
	char c;
	int nread = read(STDIN_FILENO, &c, 1);
	// read returns '\0' if no input is received after 0.1 s
	// read returns number of byte read. -1 when failure.
	if (nread == -1 && errno != EAGAIN)
		return -1;

	if (!nread) return 0;

	// Test if it is a normal keypress
	PU.updated = 1;
  if (c != '\x1b' ){ 
		KEY.key[0] = c;
		return c;
	}
	
  char seq[3];
  for (int i = 0; i < 2; ++i)
    if (read(STDIN_FILENO, &seq[i], 1) < 1)
      return '\x1b';

  if (seq[0] != '[')
    return '\x1b';

  switch (seq[1]) {
	case '5':
  if (read(STDIN_FILENO, &seq[2], 1) < 1)
		return '\x1b';
	if (seq[2]!='~') {
		return '\x1b';
	}
		KEY.key[0] = V.PAGE_UP;
		break;
	case '6':
  if (read(STDIN_FILENO, &seq[2], 1) < 1)
		return '\x1b';
	if (seq[2]!='~') {
		return '\x1b';
	}
		KEY.key[0] = V.PAGE_DOWN;
		break;
  case 'A':
		KEY.key[0] = V.ARROW_UP;
		break;
  case 'B':
		KEY.key[0] = V.ARROW_DOWN;
		break;
  case 'C':
		KEY.key[0] = V.ARROW_RIGHT;
		break;
  case 'D':
		KEY.key[0] = V.ARROW_LEFT;
		break;
  case 'H':
		KEY.key[0] = V.HOME_KEY;
		break;
  case 'F':
		KEY.key[0] = V.END_KEY;
		break;
  default:
    return '\x1b';
  }
	if (KEY.key[0] != '\x1b')
		return KEY.key[0];
	else return '\x1b';
}

int editorProcessKeyPress(void) {
	unsigned int c = KEY.key[0];
	if ( c == '\0')
		return 0;
 	if (c == CTRL_KEY('q')) {
		clearScreen();
		// Quit the program
		PU.running = 0;
		}
	else if (c == CTRL_KEY('z')){
		E.offsety = TEXTBUF.size - E.screenrows;
		E.cy = 0;
		}
	else if (c == CTRL_KEY('x')){
		E.offsety = 0;
		E.cy = E.screenrows-1;
		}
	else if (c == CTRL_KEY('m')){  // same as <Enter>, 13
		textbufEnter(&TEXTBUF, E.cx + E.offsetx, E.cy+E.offsety);
		E.cx=0;
		E.cy++;
		}
	else if (c == V.ARROW_LEFT || c == V.ARROW_RIGHT || c == V.ARROW_DOWN || c == V.ARROW_UP){
		editorMoveCursor(c);
	}
	else if (c == V.PAGE_UP){
		unsigned int times = E.screenrows;
		while (times--)
			editorMoveCursor(V.ARROW_UP);
	}
	else if (c == V.PAGE_DOWN){
		unsigned int times = E.screenrows;
		while (times--)
			editorMoveCursor(V.ARROW_DOWN);
	}
	else if (c == V.HOME_KEY || c == V.END_KEY || c == V.DEL_KEY)
		;
	else if (c == 127){ // Backspace
		if (E.cx+E.offsetx > 0){
			textbufDeleteChar(&TEXTBUF, E.cx+E.offsetx, E.cy+E.offsety);
			editorMoveCursor(V.ARROW_LEFT);
		} else if (E.cy + E.offsety > 0 && E.cx+E.offsetx<TEXTBUF.size){
			textbufDeleteLineBreak(&TEXTBUF, E.cy+E.offsety);
		}
	 }
	 else if (c == 27)
	 	;
	 else if (c < 1000){
	 	// Input
	 	textbufInputChar(&TEXTBUF, c, E.cx+E.offsetx, E.cy+E.offsety);
	 	editorMoveCursor(V.ARROW_RIGHT);
	 }
	return 1;
}

void editorSaveFile(char *ptr){
	// 0644 is octal, equivalent to 110100100 in binary
	// Owner can read and write, all other can only read.
	int fd = open(ptr, O_CREAT | O_WRONLY | O_TRUNC, 0644);	
	if (fd == -1){
		const int message_size = 64;
		char *message = (char*)calloc(message_size, sizeof(char));
		snprintf(message, message_size, "Failed to open or create file: %s", ptr);
		die(message);
	}
	for (unsigned int i = 0; i < TEXTBUF.size; i++){
		write(fd, TEXTBUF.linebuf[i], strlen(TEXTBUF.linebuf[i]));
		write(fd, "\n", 1);
	}
	close(fd);
	return;
}

static int appendWelcomeMessage(struct abuf *ptr) {
  struct abuf *abptr = ptr;
  char welcome[80];
  // KILO_VERSION defined in main.c
  // snprintf is form <stdio.h>
  unsigned int welcomelen =
      snprintf(welcome, sizeof(welcome), "Kilo Editor -- Version %d.%d.%d",
               KILO_VERSION_MAJOR, KILO_VERSION_MINOR, KILO_VERSION_PATCH);
  if (welcomelen > E.screencols)
    welcomelen = E.screencols;

  // Center the Message
  unsigned int padding = (E.screencols - welcomelen) / 2;
  if (padding) {
    abAppend(abptr, "~", 1);
    padding--;
  }
  while (padding--)
    abAppend(abptr, " ", 1);

  abAppend(abptr, welcome, welcomelen);

  return 1;
}

/*** Output ***/
void editorDrawRows(struct abuf *abptr) {
  for (unsigned int nrows = 0; nrows < E.screenrows-1; nrows++) {  // number of iteration is siginificant!
    // the line number of the row to be drawn
    const unsigned int n_rows_to_draw = nrows + E.offsety;
    if (n_rows_to_draw >= TEXTBUF.size) {
      // abAppend(abptr, "~", 1);
    } else if (nrows == E.screenrows-2){ // For debugging purpose
			const int buf_size = 100;
			char *buf = (char*)malloc(buf_size);
			snprintf(buf, buf_size, 
						"E.cx: %d; E.cy: %d; E.offsetx: %d; E.offsety: %d; rows: %d; cols: %d",
						E.cx, E.cy, E.offsetx, E.offsety, E.screenrows, E.screencols);
			abAppend(DEB.debugString, buf, strlen(buf));
			free(buf);
			abAppend(abptr, DEB.debugString->b, DEB.debugString->len);	
			abFree(DEB.debugString);
		} else {
      if (TEXTBUF.linebuf != NULL) {
        // temp points to the string of the row to be drawn.
        char *temp = *(TEXTBUF.linebuf + n_rows_to_draw);
        const unsigned int stringlen = strlen(temp);

        // For calculate the spaces for direction scrolling.
        const unsigned int xoffset = E.offsetx >= stringlen ? stringlen : E.offsetx;
        temp += xoffset;

        // Calculate the correct display length of the buffer
        unsigned int bufferlen = stringlen - xoffset; // same as strlen(temp)
        bufferlen = (bufferlen >= E.screencols) ? E.screencols - 1 : bufferlen;

        // abAppend(abptr, " ", 1);  // The space before the Line.
        abAppend(abptr, temp, bufferlen);
      }
    }
		if (nrows!=E.screencols-2) abAppend(abptr, "\r\n", 2);
  }
}

void editorRefreshScreen(void) {
  // init append buffer
  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6); // Hide cursor

  editorDrawRows(&ab);

  // Move mouse to correct position
  char buf[32];
	// move cursor, row:cols; top left is 1:1
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1); 
  abAppend(&ab, buf, strlen(buf)); // To corrected position
                                   // strlen is from <string.h>

  abAppend(&ab, "\x1b[?25h", 6); // Show cursor
  write(STDIN_FILENO, "\x1b[2J\x1b[H", 7);  // erase entire screen
  write(STDIN_FILENO, ab.b, ab.len);
  abFree(&ab);
}

int editorScrollDown(void) {
  E.offsety++;
  return 1;
}

int editorScrollUp(void) {
	E.offsety--;
	return 1;
}

int editorScrollLeft(void) {
  if (E.offsetx >= 1)
    E.offsetx--;
  return 1;
}

int editorScrollRight(void) {
  E.offsetx++;
  return 1;
}

int editorMoveCursorXTo(unsigned int x){
	E.cx = x;
	return 1;
}

int editorMoveCursorYTo( unsigned int y){
	E.cy = y;
	return 1;
}

int editorMoveCursor(int key) {
  switch (key) {
  case 1065: // up
		if (E.cy>0) E.cy--;
    else
      editorScrollUp();
    return 0;
  case 1066:  //down 
    if (E.cy <  E.screenrows -1)
      E.cy++;
    else
      editorScrollDown();
    return 0;
  case 1067: //left
    if (E.cx > 0) // padding
      E.cx--;
		else 
      editorScrollLeft();
    return 0;
  case 1068: //right
    if (E.cx < E.screencols - 1)
      E.cx++;
		else 
      editorScrollRight();
    return 0;
  default:
    return -1;
  }
  return -1;
}
