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

 // `DELETE`: \x1b[3~
 // `PAGE_UP`: \x1b[5~
 // `PAGE_DOWN`: \x1b[6~
  switch (seq[1]) {
    case '5':
    case '6':
    case '3':
    if (read(STDIN_FILENO, &seq[2], 1) < 1)
      return '\x1b';
    else if (seq[2]!='~') 
      return '\x1b'; 
    else if (seq[1] == '3')
      KEY.key[0] = DEL_KEY;
    else if (seq[1] == '5')
      KEY.key[0] = PAGE_UP;
    else if (seq[1] == '6')
      KEY.key[0] = PAGE_DOWN;
    break;
  case 'A':
		KEY.key[0] = ARROW_UP;
		break;
  case 'B':
		KEY.key[0] = ARROW_DOWN;
		break;
  case 'C':
		KEY.key[0] = ARROW_RIGHT;
		break;
  case 'D':
		KEY.key[0] = ARROW_LEFT;
		break;
  case 'H':
		KEY.key[0] = HOME_KEY;
		break;
  case 'F':
		KEY.key[0] = END_KEY;
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
	// if ( c == '\0')
	// 	return 0;
 // 	if (c == CTRL_KEY('q')) {
	// 	clearScreen();
	// 	// Quit the program
	// 	PU.running = 0;
	// 	}
	// else if (c == CTRL_KEY('z')){
	// 	E.offsety = TEXTBUF.size - E.screenrows;
	// 	E.cy = 0;
	// 	}
	// else if (c == CTRL_KEY('x')){
	// 	E.offsety = 0;
	// 	E.cy = E.screenrows-1;
	// 	}
	// else if (c == 13){  // same as CTRL_KEY('m')
	// 	textbufEnter(&TEXTBUF, E.cx + E.offsetx, E.cy+E.offsety);
	// 	E.cx=0;
	// 	E.cy++;
	// 	}
	// else if (c == V.ARROW_LEFT || c == V.ARROW_RIGHT || c == V.ARROW_DOWN || c == V.ARROW_UP){
	// 	editorMoveCursor(c);
	// }
	// else if (c == V.PAGE_UP){
	// 	unsigned int times = E.screenrows;
	// 	while (times--)
	// 		editorMoveCursor(V.ARROW_UP);
	// }
	// else if (c == V.PAGE_DOWN){
	// 	unsigned int times = E.screenrows;
	// 	while (times--)
	// 		editorMoveCursor(V.ARROW_DOWN);
	// }
	// else if (c == V.HOME_KEY || c == V.END_KEY || c == V.DEL_KEY)
	// 	;
	// else if (c == 127){ // Backspace
	// 	if (E.cx+E.offsetx > 0){
	// 		textbufDeleteChar(&TEXTBUF, E.cx+E.offsetx, E.cy+E.offsety);
	// 		editorMoveCursor(V.ARROW_LEFT);
	// 	} else if (E.cy + E.offsety > 0 && E.cx+E.offsetx<TEXTBUF.size){
	// 		textbufDeleteLineBreak(&TEXTBUF, E.cy+E.offsety);
	// 	}
	//  }
	//  else if (c == 27)
	//  	;
	//  else if (c < 1000){
	//  	// Input
	//  	textbufInputChar(&TEXTBUF, c, E.cx+E.offsetx, E.cy+E.offsety);
	//  	editorMoveCursor(V.ARROW_RIGHT);
	//  }
  
  switch (c){
    case '\0':
      break;
    case CTRL_KEY('q'):
      clearScreen();
      PU.running = 0;
      break;
    case 13:  // Enter key, or ctrl('m')
      textbufEnter(&TEXTBUF, E.cx + E.offsetx, E.cy+E.offsety);
      E.cx = 0;
      E.cy++;
      break;
    case ARROW_LEFT:
    case ARROW_RIGHT:
    case ARROW_DOWN:
    case ARROW_UP:
      editorMoveCursor(c);
      break;
    case PAGE_UP:
    case PAGE_DOWN:
      for (unsigned int i = 0; i < E.screenrows; i++)
        editorMoveCursor(c);
      break;
    case DEL_KEY: {
        const unsigned int len = strlen(TEXTBUF.linebuf[E.cy+E.offsety]);
        if ((E.cx+E.offsety) < len)
          textbufDeleteChar(&TEXTBUF, E.cx+E.offsetx+1, E.cy+E.offsety);
        break;
      }
    case HOME_KEY:
    case END_KEY:
      break;
    case 127:  // Backspace
      if (E.cx+E.offsetx > 0){
        textbufDeleteChar(&TEXTBUF, E.cx+E.offsetx, E.cy+E.offsety);
        editorMoveCursor(V.ARROW_LEFT);
      } else if (E.cy + E.offsety > 0 && E.cx+E.offsetx<TEXTBUF.size){
        textbufDeleteLineBreak(&TEXTBUF, E.cy+E.offsety);
      }
    case 27:  // escape
      break;
    default:
      // for now ignore all control-keys
      if (c < 27) 
        break;
      // special characters are defined to be greater than 1000
      else if (c < 1000){ 
        textbufInputChar(&TEXTBUF, c, E.cx+E.offsetx, E.cy+E.offsety);
        editorMoveCursor(V.ARROW_RIGHT);
      }
      break;
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
    if (write(fd, TEXTBUF.linebuf[i], strlen(TEXTBUF.linebuf[i])) == -1)
      die("Failed to Write to Disk!");
		if (write(fd, "\n", 1) == -1)
      die("Failed to Write to Disk!");
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

void screenBufferAppendDebugInformation(struct abuf *abptr){
  const int buf_size = 100;
  char *buf = (char*)malloc(buf_size);
  // snprintf(buf, buf_size, 
  //          "E.cx: %d; E.cy: %d; strlen: %ld",
  //          E.cx, E.cy, strlen(TEXTBUF.linebuf[E.cy + E.offsety]));
  snprintf(buf, buf_size, 
           "E.cx: %d; E.cy: %d; E.offsetx: %d; E.offsety: %d; rows: %d; cols: %d",
           E.cx, E.cy, E.offsetx, E.offsety, E.screenrows, E.screencols);
  abAppend(DEB.debugString, buf, strlen(buf));  // Append message to the global struct
  free(buf);
  abAppend(abptr, DEB.debugString->b, DEB.debugString->len);	
  abFree(DEB.debugString);
}

/*** Output ***/
void editorDrawRows(struct abuf *abptr) {
  for (unsigned int nrows = 0; nrows < E.screenrows ; nrows++) {  // number of iteration is siginificant!
    // the line number of the row to be drawn
    const unsigned int n_rows_to_draw = nrows + E.offsety;

    // Create left margin (line number)
    char *leftMargin = (char *)calloc(E.leftMarginSize, 1);
    int lineNumber;
    if (nrows ==  E.cy){
      lineNumber = E.offsety + E.cy;
      snprintf(leftMargin, E.leftMarginSize, "%d", lineNumber);
      // As in vim, the line number in the current line is aliged to right
      // Create necessary paddings
      if (strlen(leftMargin)<E.leftMarginSize){
        // recall E.leftMarginSize is seted to 1 more than the maximum 
        // line number to include the extra space
        for (size_t i = 0; i < E.leftMarginSize-strlen(leftMargin)-1; i++){
          abAppend(abptr, " ", 1);
        }
      }
      abAppend(abptr, "\x1b[1m", 4);
      abAppend(abptr, leftMargin, strlen(leftMargin));
      abAppend(abptr, "\x1b[0m", 4);
      // the extra space
      abAppend(abptr, " ", 1);
    } else {
      int temp;
      lineNumber = (temp = (nrows - E.cy)) > 0 ? temp : -temp; 
      snprintf(leftMargin, E.leftMarginSize, "%d", lineNumber);
      abAppend(abptr, leftMargin, strlen(leftMargin));
      // Create necessary paddings
      // recall E.leftMarginSize is seted to 1 more than the maximum 
      for (size_t i = 0; i < E.leftMarginSize-strlen(leftMargin)-1; i++){
        abAppend(abptr, " ", 1);
      }
      // The extra space 
      abAppend(abptr, " ", 1);
    }

    if (n_rows_to_draw >= TEXTBUF.size) {
      // abAppend(abptr, "~", 1);
    }
    else if (nrows == E.screenrows-4){ // For debugging purpose
      screenBufferAppendDebugInformation(abptr);
		}
    else {
      if (TEXTBUF.linebuf != NULL) {
        // temp points to the string of the row to be drawn.
        char *temp = *(TEXTBUF.linebuf + n_rows_to_draw);
        const unsigned int stringlen = strlen(temp);

        // For calculate the spaces for direction scrolling.
        const unsigned int xoffset = E.offsetx >= stringlen ? stringlen : E.offsetx;
        temp += xoffset;

        // Calculate the correct display length of the buffer
        unsigned int bufferlen = stringlen - xoffset; // same as strlen(temp)
        bufferlen = (bufferlen >= E.screencols - E.leftMarginSize) ? 
          E.screencols - E.leftMarginSize : bufferlen;

        // abAppend(abptr, " ", 1);  // The space before the Line.
        abAppend(abptr, temp, bufferlen);
        // abAppend(abptr, "\r\n", 2);
      }
    }
		if (nrows<E.screenrows - 1) abAppend(abptr, "\r\n", 2);
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
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + E.leftMarginSize  + 1); 
  // abAppend(&ab, "\x1b[H", 3); 
  abAppend(&ab, buf, strlen(buf)); // To corrected position
                                   // strlen is from <string.h>

  // TESTING: 
  // abAppend(&ab, "\x1b[35;70H", 10); // Go to last line
  // abAppend(&ab, "\x1b[?25h", 6); // Go to middle of last line
  
  abAppend(&ab, "\x1b[?25h", 6); // Show cursor
  // tracker();
  write(STDIN_FILENO, "\x1b[2J\x1b[H", 7);  // erase entire screen
  write(STDIN_FILENO, ab.b, ab.len);
  abFree(&ab);
}

void editorScrollDown(void) {
    E.offsety++;
}

void editorScrollUp(void) {
  	E.offsety--;
}

void editorScrollLeft(void) {
  E.offsetx--;
}

void editorScrollRight(void) {
  E.offsetx++;
}

void editorMoveCursorXTo(unsigned int x){
	E.cx = x;
}

void editorMoveCursorYTo( unsigned int y){
	E.cy = y;
}

// TODO: Needs improvement: Current function does intermix 
// TODO: the move cursor function and screen scrolling
// BUG: bug remains
int editorMoveCursor(int key) {
  switch (key) {
  case ARROW_UP: 
  case PAGE_UP:
		if (E.cy>0) 
        E.cy--;
    else if (E.offsety > 0)
      editorScrollUp();
    return 0;
  case ARROW_DOWN:
  case PAGE_DOWN:
    if (E.cy <  E.screenrows -1)
      E.cy++;
    else if (E.cy + E.offsety < TEXTBUF.size )
      editorScrollDown();
    return 0;
  case ARROW_LEFT: 
    if (E.cx > 0) 
      E.cx--;
		else if (E.cx + E.offsetx > 0)
      editorScrollLeft();
    return 0;
  case ARROW_RIGHT: 
		if ((E.cx + E.offsetx) < strlen(TEXTBUF.linebuf[E.cy + E.offsety]) ) 
      E.cx++;
    else if (E.cx >= E.screencols - 1)
      editorScrollRight();
    return 0;
  default:
    return -1;
  }
  return -1;
}

// Set editorConfig according to number of lines stored in textbuf
void editorSetMarginSize(struct editorConfig *ptr,textbuf *ptrtb){
  int line = ptrtb->size; 
  int counter;
  // Find out the digits of greatest linenumber
  for (counter = 0; line>0; line /= 10, counter ++);
  // One more space for padding ' '
  ptr->leftMarginSize = counter + 1;
  return;
}
