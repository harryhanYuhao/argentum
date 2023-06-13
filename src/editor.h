#ifndef EDITOR_H
#define	EDITOR_H 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int editorReadKey(void);
int editorProcessKeyPress(void);
void editorRefreshScreen(void);
void editorInit(void);
void editorSaveFile(char *);
void editorOpen(const char *);
int editorMoveCursor(int);
void editorMoveCursorXTo(unsigned int);
void editorMoveCursorYTo(unsigned int);
void editorSetMarginSize(struct editorConfig *, textbuf *);

#endif // For EDITOR_H
