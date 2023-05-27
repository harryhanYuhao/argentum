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
static int addtextbuf(textbuf * ptrtb, char * string){
	(ptrtb->size)++;
	int stringLength = strlen(string);
	char ** ptrbuf = (char**)realloc(ptrtb->linebuf, (ptrtb->size)*sizeof(char*));
	ptrtb->linebuf = ptrbuf;
	(ptrtb->linebuf)[ptrtb->size - 1] = (char*)calloc(stringLength + 1, 1);
	memcpy((ptrtb->linebuf)[ptrtb->size - 1], string, stringLength - 1);
	(ptrtb->linebuf)[ptrtb->size - 1][stringLength]= '\0';
	return 1;
}

int textbufRead(textbuf *tb, FILE *fp) {
	char * buf;
	size_t size;

	buf = NULL; size = 0;
	while (getline(&buf, &size, fp)>=1){
		addtextbuf(tb, buf);
		free(buf);
		buf = NULL; size = 0;
	}
	return 1;
}

void textbufInputChar(textbuf *ptrtb, char inputChar, int x, int y){
	char *linebuf = ptrtb->linebuf[y+1];
	int len = strlen(linebuf);  
	if (x <= len+1  && x>3){
	// strlen does not count the final null terminator.
	linebuf=realloc(linebuf, len+2); // extra space for null terminator
	// memmove: <string.h>, c11
	// memmove(dest, src, n)
	memmove(&linebuf[x], &linebuf[x-1], (len-x+1)*sizeof(char));
	linebuf[x-1]=inputChar; // Assign Character
	linebuf[len+1] = '\0'; 
	ptrtb->linebuf[y+1]=linebuf;
	}
}

void textbufDeleteChar(textbuf *ptrtb, int x, int y){
	char *linebuf = ptrtb->linebuf[y+1];
	int len = strlen(linebuf);  
		// the left padding length shall be included
		// needs to take account of the null Character
	if (x<= len+1 && x>4){  
	linebuf=realloc(linebuf, len+2); // extra space for null terminator
	memmove(&linebuf[x-2], &linebuf[x-1], (len-x+2)*sizeof(char));
	linebuf=realloc(linebuf, len); // extra space for null terminator
	ptrtb->linebuf[y+1]=linebuf;
	}
}

void keyInit(struct key *K){
	K->key = 0;
	K->special = 0;
	if (K->key!=0 || K->special!=0) 
		die("Failed to Init struct key!");
}

void programUtilsInit(struct programUtils *p){
  p->running = 1;
  p->updated = 1;
}
