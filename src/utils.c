#include "utils.h"

void clearScreen(void) {
  write(STDIN_FILENO, "\x1b[2J",
        4); // Erase in display, option 2, the whole screen, cursor do not move
  write(STDIN_FILENO, "\x1b[H",
        3); // Place the cursor the to top left of the screen
}

void die(const char *s) {
  clearScreen();

  perror(s); // From <stdio.h>
  // The termial is in RAW mode. perrors will return \n but not \r
  write(STDIN_FILENO, "\r", 1);
  exit(1); // Exit with 1. From <stdlib.h>
}

#ifdef ABUF_INIT
void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, len + ab->len);
  if (new == NULL)
    die("Fail to realloc memory for function abAppend");
  memcpy(&new[ab->len], s, len); // From <string.h>
  ab->b = new;
  ab->len += len;
}

void abFree(struct abuf *ab) { free(ab->b); }
#endif

// an implementation of getline()
// getline() is included in see man getline(3) which contains the following
//
// Since glibc 2.10:
//     _POSIX_C_SOURCE >= 200809L
// Before glibc 2.10:
//     _GNU_SOURCE

#if !defined _POSIX_C_SOURCE || !defined _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
ssize_t getline(char **restrict buffer, size_t *restrict size,
                FILE *restrict fp) {
  register int c;
  register char *cs = NULL;

  if (cs == NULL) {
    register int length = 0;
    while ((c = getc(fp)) != EOF) {
      cs = (char *)realloc(cs, ++length + 1);
      if ((*(cs + length - 1) = c) == '\n') {
        *(cs + length) = '\0';
        *buffer = cs;
        break;
      }
    }
    return (ssize_t)(*size = length);
  } else {
    while (--(*size) > 0 && (c = getc(fp)) != EOF) {
      if ((*cs++ = c) == '\n')
        break;
    }
    *cs = '\0';
  }
  return (ssize_t)(*size = strlen(*buffer));
}
#endif
