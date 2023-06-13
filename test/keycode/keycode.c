#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

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
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN); // Ctrl-c can terminate the program
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag &= ~(CS8);

  raw.c_cc[VMIN] = 0;  // read() returns nothing after timeout 
  raw.c_cc[VTIME] = 1; // Timeout after 0.1 s

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    perror("tcsetattr");
  }
}

int main(int argc, char **argv) {
  enableRAWMode();
  char temp;
  while (1) {
    if (read(STDIN_FILENO, &temp, 1)) {
      char *ctemp = (char *)malloc(16);
      switch (temp) {
      case 27:
        snprintf(ctemp, 4, "ESC");
        break;
      case 13:
        snprintf(ctemp, 6, "ENTER");
        break;
      case 127:
        snprintf(ctemp, 10, "BACKSPACE");
        break;
      default:
        snprintf(ctemp, 2, "%c", temp);
      }
      printf("Code: %d (\'%s\')\r\n", temp, ctemp);
      if (temp == 13) printf("\n");
    }
  }
  return 0;
}
