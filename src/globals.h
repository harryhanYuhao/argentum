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
  unsigned int size; // Total number of lines
  char **linebuf;    // A pointer storing pointer to line buffer
} textbuf;           // textbuffer holding all lines.

void textbufInputChar(textbuf *, char, int x, int y);
void textbufDeleteChar(textbuf *, int x, int y);
void textbufEnter(textbuf *, unsigned int x, unsigned int y);

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

#endif // for GLOBALS_H
