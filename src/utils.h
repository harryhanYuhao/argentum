#ifndef UTILS_H
#define UTILS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// append buffer: my custom mutable strings in C
#ifndef ABUF_INIT
#define ABUF_INIT                                                              \
  { NULL, 0 }

struct abuf {
  char *b;
  int len;
};

void abAppend(struct abuf *, const char *s, int len);
void abFree(struct abuf *ab);
#endif

// Utilities functions
void clearScreen(void);

#include <errno.h>
void die(const char *);

// an implementation of getline()
// getline() is included in 
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
                FILE *restrict fp);
#endif

#endif // For UTILS_H
