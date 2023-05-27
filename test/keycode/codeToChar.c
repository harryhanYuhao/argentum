#include <stdio.h>
#include <stdlib.h>

int main() {//int argc, char *argv[]
	int size = 0;
	printf("%c\n", 40);
	while(1){
		char *line=(char*) malloc(10);
		getline(&line, &size, stdin);
		int b = 1;
		sscanf(line, "%d", &b);
		printf("%c\n", b);
		free(line);
	}
	return 0;
}
