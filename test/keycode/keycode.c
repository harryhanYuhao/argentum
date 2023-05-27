#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>

struct termios origin;

void disableRAWMode(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origin) == -1) {
    perror("error occur in function disableRAWMode");
  }
}

void enableRAWMode(void) {
  struct termios raw;
  if (tcgetattr(STDIN_FILENO, &raw) == -1) {
		// STDIN_FILENO is the standard input
    perror("tcgetattr");
	}
  if (tcgetattr(STDIN_FILENO, &origin) == -1) {
    perror("tcgetattr");
  }
  atexit(&disableRAWMode); // From <stdlib.h> Execute the function when the
                           // program exits.
	
	// raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
	raw.c_lflag &= ~(ECHO|ICANON|IEXTEN); // Ctrl-c can terminate the program
	raw.c_iflag &= ~(IXON|ICRNL|BRKINT|INPCK|ISTRIP);
  raw.c_oflag &= ~(OPOST); 
  raw.c_cflag &= ~(CS8);

  raw.c_cc[VMIN] = 0;  // what read() returns after timeout
  raw.c_cc[VTIME] = 1; // Timeout after 0.1 s

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    perror("tcsetattr");
  }
}


int main(int argc, char **argv){
	enableRAWMode();
	char temp;
	while(1){
		if (read(STDIN_FILENO, &temp, 1))
			printf("Code: %d\r\n", temp);
		// sleep(1);
	}
	return 0;
}
