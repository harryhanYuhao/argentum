#include <stdio.h>
#include <stdlib.h>

int main() {//int argc, char *argv[]
	FILE *fp = fopen("samplefile.txt", "w"); 
	for (int i = 0; i < 200; i++){
		fprintf(fp, "%d: ", i);
		for (int j = 0; j < i; j++){
			fprintf(fp, "@");
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}
