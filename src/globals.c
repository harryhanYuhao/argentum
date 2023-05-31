#include "globals.h"
#include "utils.h"

struct editorConfig E;
struct programUtils PU;
struct key KEY;
textbuf TEXTBUF;

void textbufInit(textbuf *t) {
  t->size = 0;
  t->linebuf = NULL;
  if (t->size != 0 || t->linebuf != NULL)
    die("Failed to Init Textbuf!");
}

#include <string.h>
static int addtextbuf(textbuf *ptrtb, char *string) {
  (ptrtb->size)++;
  int stringLength = strlen(string);
  ptrtb->linebuf =
      (char **)realloc(ptrtb->linebuf, (ptrtb->size) * sizeof(char *));
  // ptrtb->linebuf = ptrbuf;
  (ptrtb->linebuf)[ptrtb->size - 1] = (char *)calloc(stringLength, 1);
  memcpy((ptrtb->linebuf)[ptrtb->size - 1], string,
         stringLength - 1); // ignore '\n', which is read by getline()
  (ptrtb->linebuf)[ptrtb->size - 1][stringLength - 1] = '\0';
  return 1;
}

int textbufRead(textbuf *tb, FILE *fp) {
  char *buf;
  size_t size;

  buf = NULL;
  size = 0;
  while (getline(&buf, &size, fp) >= 1) {
    addtextbuf(tb, buf);
    free(buf);
    buf = NULL;
    size = 0;
  }
  return 1;
}

void textbufInputChar(textbuf *ptrtb, char inputChar, int x, int y) {
  char *linebuf = ptrtb->linebuf[y];
  int len = strlen(linebuf);
  if (x <= len && x >= 0) {
    // strlen does not count the final null terminator.
    linebuf = realloc(linebuf, len + 2); // extra space for null terminator
    // memmove: <string.h>, c11
    // memmove(dest, src, n)
    // This is the index:
    // 0, 1, 2, ... , x, x+1, ... len
    // there are 'x' elements from 0 to x,
    // 'len-x' element from x to the end of array
    memmove(&linebuf[x + 1], &linebuf[x], (len - x) * sizeof(char));
    linebuf[x] = inputChar; // Assign Character
    linebuf[len + 1] = '\0';
    ptrtb->linebuf[y] = linebuf;
  }
}

void textbufDeleteChar(textbuf *ptrtb, int x, int y) {
  char *linebuf = ptrtb->linebuf[y];
  int len = strlen(linebuf);
  // the left padding length shall be included
  // needs to take account of the null Character
  if (x <= len + 1 && x > 0) {
    linebuf = realloc(linebuf, len + 2); // extra space for null terminator
    memmove(&linebuf[x - 1], &linebuf[x], (len - x + 1) * sizeof(char));
    linebuf = realloc(linebuf, len); // extra space for null terminator
    ptrtb->linebuf[y] = linebuf;
  }
}

// Insert New line above with only '\0'
// Input E.cy + E.offsety
void textbufEnter(textbuf *ptrtb, unsigned int x, unsigned int y) {
  const unsigned int len = ptrtb->size;
  if (y <= len + 1) {
    ptrtb->size++;
    char **linebuf = ptrtb->linebuf;
    linebuf = realloc(linebuf, (len + 1) * sizeof(char *));
    ptrtb->linebuf = linebuf; // in case of realloc changed the pointer
    memmove(&linebuf[y + 1], &linebuf[y], (len - y) * sizeof(char *));
    // calloc is required to create a new pointer with allocated space
    linebuf[y] = (char *)calloc(1 + x, sizeof(char));
    // Modify the line above
    memcpy(linebuf[y], linebuf[y + 1], x * sizeof(char));
    linebuf[y][x] = '\0';
    // Modify the line below
    const unsigned int strLength = strlen(linebuf[y + 1]);
    memmove(linebuf[y + 1], linebuf[y + 1] + x, strLength - x);
    linebuf[y + 1][strLength - x] = '\0';
    linebuf[y + 1] = realloc(linebuf[y + 1], strLength - x + 1);
  }
}

void keyInit(struct key *K) {
  K->key = 0;
  K->special = 0;
  if (K->key != 0 || K->special != 0)
    die("Failed to Init struct key!");
}

void programUtilsInit(struct programUtils *p) {
  p->running = 1;
  p->updated = 1;
}
