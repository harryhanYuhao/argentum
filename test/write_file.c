#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {//int argc, char *argv[]
  srand(time(0));
	FILE *fp = fopen("samplefile.txt", "w"); 
	for (int i = 0; i < 200; i++){
		// fprintf(fp, "%d: ", i);
		for (int j = 0; j < i; j++){
      char temp[2];
      temp[0] = (char) ((2*i*3*j+rand())%91 + 34);
      temp[1] = '\0';
			fprintf(fp, temp);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return 0;
}
