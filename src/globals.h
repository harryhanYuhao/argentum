#ifndef GLOBALS_H
#define GLOBALS_H 1

#ifndef CTRL_KEY
#define CTRL_KEY(k) ((k)&0x1f)
#endif

#define KILO_VERSION_MAJOR 0
#define KILO_VERSION_MINOR 0
#define KILO_VERSION_PATCH 1

#include <termios.h>
typedef struct {
  unsigned int size;
  char *chars;
} erow; // editor row

typedef struct {
  // TODO: Change size to numlines
  unsigned int size;       // Total number of lines
  char **linebuf; // A pointer storing pointer to line buffer
} textbuf;        // textbuffer holding all lines.

#include <stddef.h>
void textbufInit(textbuf *);

#include <stdio.h>
int textbufRead(textbuf *, FILE *);

enum editorKey {
  ARROW_LEFT = 100,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  PAGE_UP,
  PAGE_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY
};

struct editorConfig {
  unsigned int cx, cy;     // cursor position. cx horizantol, cy vertical
  unsigned int screenrows; // number of rows that fit in the screen
  unsigned int screencols; // number of columns that fit in the screen
  unsigned int offsetx;  // Display offset, x direction
  unsigned int offsety;
	unsigned int mode;  // Indicator for mode 
  struct termios orig_termios;
};

struct programUtils {
  unsigned int running;
  unsigned int updated;
};

struct key {
	unsigned int key;
	unsigned int special;
};

void textbufInputChar(textbuf *, char, int x, int y);
void textbufDeleteChar(textbuf *, int x, int y);
void textbufNewLineAbove(textbuf *, int x, int y);

void keyInit(struct key*);
void programUtilsInit(struct programUtils *);
#endif // for GLOBALS_H
