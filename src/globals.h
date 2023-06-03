#ifndef GLOBALS_H
#define GLOBALS_H 1

#ifndef CTRL_KEY
#define CTRL_KEY(k) ((k)&0x1f)
#endif

#define KILO_VERSION_MAJOR 0
#define KILO_VERSION_MINOR 0
#define KILO_VERSION_PATCH 1

#include "utils.h"
#include <termios.h>
typedef struct {
  unsigned int size;
  char *chars;
} erow; // editor row

typedef struct {
  // TODO: Change size to numlines
  unsigned int size; // Total number of lines
  char **linebuf;    // A pointer storing pointer to line buffer
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

// It is merely a struct for holding global key values
struct keyValue {
  unsigned int ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT;
  unsigned int PAGE_UP, PAGE_DOWN;
  unsigned int DEL_KEY, END_KEY, HOME_KEY;
};

int keyValueInit(struct keyValue *);

struct debugUtil{
	struct abuf * debugString;
};

int debugUtilInit(struct debugUtil*);
// The string must be null terminated 
int debugAddMessage(struct debugUtil*, const char*); 

#endif // for GLOBALS_H
