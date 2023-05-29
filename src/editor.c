#include "utils.h"
#include "globals.h"
#include "editor.h"

extern struct editorConfig E;
extern struct programUtils PU;
extern struct key KEY;
extern textbuf TEXTBUF;

/*** FILE IO ***/
// Change to open() syscall
void editorOpen(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    char errorMessage[100];
    snprintf(errorMessage, 100, "Can not open File '%s'\r\nperrer message",
             filename);
    die(errorMessage);
  }

  textbufRead(&TEXTBUF, fp); // All lines of the file are read into TEXTBUF
  fclose(fp);
}

/*** Input ***/
/// Reads and returns the key once.
char editorReadKey(void) {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) < 1) {
    // read returns '\0' if no input is received after 0.1 s
    // read returns number of byte read. -1 when failure.
    if (nread == -1 && errno != EAGAIN)
      die("editorReadKey failed!");
  }

  if (c != '\x1b') // if no escape sequence is read.
    return c;

  char seq[3];
  for (int i = 0; i < 2; ++i)
    if (read(STDIN_FILENO, &seq[i], 1) < 1)
      return '\x1b';

  if (seq[0] != '[')
    return '\x1b';

  if (seq[1] == '5')
    return PAGE_UP;
  if (seq[1] == '6')
    return PAGE_DOWN;
  if (seq[1] == '3')
    return DEL_KEY;
  if (seq[1] == '1')
    return HOME_KEY;

 //  if (read(STDIN_FILENO, &seq[2], 1) < 1)
	// 	return '\x1b';
	// if (seq[2]!='~') 
	// 	return '\x1b';
  switch (seq[1]) {
  case 'A':
    return ARROW_UP; // TESTED
  case 'B':
    return ARROW_DOWN; // TESTED
  case 'C':
    return ARROW_RIGHT; // TESTED
  case 'D':
    return ARROW_LEFT; // TESTED
  case 'H':
    return HOME_KEY; // TESTED
  case 'F':
    return END_KEY;
  default:
    return '\x1b';
  }
  return '\x1b';
}

void editorProcessKeyPress(void) {
  char c = editorReadKey();
  PU.updated = 1;
  switch (c) {
  case (CTRL_KEY('q')):
    clearScreen();
		editorSaveFile("aaa.txt");
    PU.running = 0;
    break;

  case (CTRL_KEY('z')):
		E.offsety = TEXTBUF.size - E.screenrows;
		E.cy = 0;
    break;
  case (CTRL_KEY('x')):
		E.offsety = 0;
		E.cy = E.screenrows-1;
    break;
	case (CTRL_KEY('n')):
		break;

  case ARROW_LEFT:
  case ARROW_RIGHT:
  case ARROW_DOWN:
  case ARROW_UP:
    editorMoveCursor(c);
    break;

  case PAGE_UP: // PAGE_UP, PAGE_DOWN tested
  case PAGE_DOWN: {
    unsigned int times = E.screenrows;
    while (times--)
      editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    break;
  }
  case HOME_KEY:
  case END_KEY:
  case DEL_KEY:
    break;
	case 127: // Backspace
		textbufDeleteChar(&TEXTBUF, E.cx+E.offsetx, E.cy+E.offsety);

		editorMoveCursor(ARROW_LEFT);
		// textbufInputChar(&TEXTBUF, 'a', E.cx+E.offsetx, E.cy+E.offsety);
		break;
	case 8:
		textbufInputChar(&TEXTBUF, 'a', E.cx+E.offsetx, E.cy+E.offsety);
		break;
  default:
		// Input
		textbufInputChar(&TEXTBUF, c, E.cx+E.offsetx, E.cy+E.offsety);
		editorMoveCursor(ARROW_RIGHT);
    break;
  }
}

void editorSaveFile(char *ptr){
	// 0644 is octal, equivalent to 110100100 in binary
	// Owner can read and write, all other can only read.
	int fd = open(ptr, O_CREAT | O_RDWR, 0644);	
	if (fd == -1){
		char message [64];
		// sprintf shall be generally avoided
		snprintf(message, sizeof(message), "Failed to open file: %s", ptr);
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
						"E.cx: %d; E.cy: %d; E.offsetx: %d; E.offsety: %d; screenrows: %d; screencols: %d",
						E.cx, E.cy, E.offsetx, E.offsety, E.screenrows, E.screencols);
			abAppend(abptr, buf, strlen(buf));	
			free(buf);
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

// TODO: Protection mechanism (stop from scrolling too far away from text 
// TODO: Needs to be reworked
int editorMoveCursor(char key) {
  PU.updated = 1;
  switch (key) {
  case ARROW_UP:
		if (E.cy>0) E.cy--;
    else
      editorScrollUp();
    return 0;
  case ARROW_DOWN:
    if (E.cy <  E.screenrows -1)
      E.cy++;
    else
      editorScrollDown();
    return 0;
  case ARROW_LEFT:
    if (E.cx > 0){ // padding
      E.cx--;
    if (E.cx < E.screencols / 4)
      editorScrollLeft();
			}
    return 0;
  case ARROW_RIGHT:
    if (E.cx < E.screencols - 1)
      E.cx++;
    if (E.cx > 3 * E.screencols / 4)
      editorScrollRight();
    return 0;
  default:
    return -1;
  }
  return -1;
}
