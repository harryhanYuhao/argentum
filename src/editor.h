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
int editorMoveCursorXTo(unsigned int);
int editorMoveCursorYTo(unsigned int);

#endif // For EDITOR_H
