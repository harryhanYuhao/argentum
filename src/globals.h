#ifndef GLOBALS_H
#define GLOBALS_H 1

#ifndef CTRL_KEY
#define CTRL_KEY(k) ((k)&0x1f)
#endif

#define KILO_VERSION_MAJOR 0
#define KILO_VERSION_MINOR 0
#define KILO_VERSION_PATCH 1

enum {
  ARROW_UP = 1065, 
  ARROW_DOWN = 1066,
  ARROW_LEFT = 1067,
  ARROW_RIGHT = 1068,
  PAGE_UP = 1053, 
  PAGE_DOWN = 1054,
  DEL_KEY = 1051, 
  END_KEY = 1070, 
  HOME_KEY = 1072,
};

#include "utils.h"
#include <termios.h>

typedef struct {
  // TODO: Change size to numlines
  unsigned int size; // Total number of lines
  char **linebuf;    // A pointer storing pointer to line buffer
  int *lineLength;  // lineLength[n] = strlen(linebuf[n])
} textbuf;           // textbuffer holding all lines.


#include <stddef.h>
void textbufInit(textbuf *);
int textbufGetNthLineLength(textbuf *, int);

#include <stdio.h>
int textbufRead(textbuf *, FILE *);

int textbufInitForEmptyFile(textbuf *);
void textbufInputChar(textbuf *, char, int, int);
void textbufDeleteChar(textbuf *, int, int);
void textbufEnter(textbuf *, unsigned int, unsigned int);
int textbufDeleteLine(textbuf *, unsigned int);
int textbufDeleteLineBreak(textbuf *, unsigned int);

struct editorConfig {
  unsigned int cx, cy;     // cursor position. cx horizantol, cy vertical
  unsigned int screenrows; // number of rows that fit in the screen
  unsigned int screencols; // number of columns that fit in the screen
  unsigned int offsetx;    // Display offset, x direction
  unsigned int offsety;
  unsigned int mode; // Indicator for mode
  unsigned int leftMarginSize;
  struct termios orig_termios;
};

struct programUtils {
  unsigned int running;
  unsigned int updated;
};

void programUtilsInit(struct programUtils *);

///  The key struct can hold upto 8 keys
///  If the key value is smaller
/// than 1000, it represents the key corresponding to the ASCII code
/// key value of 1000 - 2000 representes escaped key
/// note 'ctrl(a)' = 'a' - 96 = 1
struct key {
  unsigned int key[8];
};

int keyInit(struct key *);
int keyRefresh(struct key *);

struct debugUtil{
	struct abuf * debugString;
};

int debugUtilInit(struct debugUtil*);
// The string must be null terminated 
int debugAddMessage(struct debugUtil*, const char*); 

#endif // for GLOBALS_H
