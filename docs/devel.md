# Development Documentation

## OverView Of Implementation

### Text Storage

All texts are stored in buffer `struct textbuf TEXTBUF`. Many buffer may exists at the same time.

### Screen-Cursor-Buffer Interaction

Screen will show the approipriate portion of the buffer with auxillary informations; pressing key will modify the buffer accordingly, orchange the way how the textbuffer is presented on screen. 

The cursor position is the position of the cursor on screen; with the upper left corner being 1,1. The x, y position of the cursor are stored in the global variable `E.cx`, `E.cy`. The current position of the cursor is equivalent to `\x1b[1+E.cx; E.cyH.

## Screen-Curosr-Text Interaction



## Globle Variables

### `extern struct editorConfig E`

Struct defined in `global.h`.

### `extern textbuf TEXTBUF`

Struct defined in `global.h`: 

#### Struct Functions 

Functions whose named began with `textbuf` manipuate the structure textbuf with side effect, and nothing else.

`typedef struct {int size; char **linebuf;}; textbuf` 

## `editor.c` 

### `char editorReadKey(void)`

Read one key from `STDIN_FILENO`. Return its keycode and store it in the gloabl ` KEY` struct. The key code are idiosyncratically defined as below

####  Key Code 

- A key code is an integer representing one and only one key. (with few exception `<C-m>` and `<Enter>` are both represented by 13.)
- The Key Code is the ascii code if such key has a correspounding one. (Including `<backspace>, <enter>, <ctrl-key>` etc.).
- Key code of others (usually escaped keys) are idiosyncratically defined to be greater than 1000, and is defined below.

- `ARROW_UP`: 1065
- `ARROW_DOWN`: 1066
- `ARROW_LEFT`: 1067
- `ARROW_RIGHT`: 1068
- `DELETE`: 1051
- `PAGE_UP`: 1053
- `PAGE_DOWN`: 1054
- `END`: 1070
- `HOME`: 1072

- `ARROW_UP`: \x1b[A
- `ARROW_DOWN`: \x1b[B
- `ARROW_RIGHT`: \x1b[C
- `ARROW_LEFT`: \x1b[D
- `DELETE`: \x1b[3~
- `PAGE_UP`: \x1b[5~
- `PAGE_DOWN`: \x1b[6~
- `END`: \x1b[F
- `HOME`: \x1b[H

Note that the escape sequence is read in 3 parts, the first part is always
'\x1b', the second part is always '[', and the third part is the actual
escape sequence.

## DebugPrinting

API for quick debug printing, defined in `global.h`, is:

```c
//The string must be null terminated 
int debugAddMessage(struct debugUtil*, const char*); 
```

The global struct `DEB` is defined in `global.h` as well. 

Example: `debugAddMessage(&DEB, "UP!");`


## utils.h

### `struct abuf {char *b, int len};` 

This struct is used like a string, with two function `abAppend` and `abFree` to append and free the string.

- `void abAppend(struct abuf *ab, const char *s, int len)` <br>
Append `len` characters from `s` to `ab->b` (realloc memory if required). `ab->len` is updated accordingly.

- `void abFree(struct abuf *ab)`<br> Free the memory of `ab`. 

### `ssize_t getline(char **lineptr, size_t *n, FILE *stream)`

My custom ISO compliant version of `getline()`. If `*lineptr` is `NULL` or `*n` is 0, then `getline()` will allocate a buffer for storing the line. This buffer should be freed by the user program even if `getline()` failed. 

## terminal.h

### `void disableRawMode(void)`

Disable raw mode by resetting the terminal attributes. 

### `int enableRawMode(void)`

Enable raw mode by setting the terminal attributes. 

### Note on RAW Mode

The following flags are turned off to enable terminal raw mode.

```c
raw.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);
raw.c_iflag &= ~(IXON|ICRNL|BRKINT|INPCK|ISTRIP);
raw.c_oflag &= ~(OPOST);
raw.c_cflag |= (CS8);
```

Here, `raw` is a `struct termios`. The flag, functions, and structs are from the header file `termios.h`.

- `ECHO`: if on, input will be echoed back to the terminal. This flag is off in raw mode because we don't want to see the keypresses.
- `ICANON`: if on, input will be line-buffered instead of byte-buffered. This means that `STDIN_FILENO` will only send the input to the program when the user presses `Enter`.
- `ISIG`: if on, `SIGINT` (`ctrl-c`) and `SIGTSTP` (`ctrl-z`) will be sent to the program, which by default will terminate it. 
- `IEXTEN`: if on, the program will quit on `CTRL-V` or `CTRL-O`. 
- `IXON`: if on, `CTRL-S` and `CTRL-Q` will pause and resume the program respectively. 
- `ICRNL`: if on, `'\r'` will be converted to `'\n'`.
- `BRKINT`: if on, `SIGINT` will be sent to the program when `CTRL-C` is pressed.
- `INPCK`: if on, input parity checking is enabled. 
- `ISTRIP`: if on, the 8th bit of each input byte will be stripped. 
- `OPOST`: if on, output processing is enabled. `\n` will be converted to `\r\n`. 
- `CS8`: if on, character size is 8 bits per byte. 

## Useful Escape Sequences 

- "\x1b[?25l": hide cursor
- "\x1b[?25h": show cursor
- "\x1b[H": move cursor to top-left corner
- "\x1b[2J": clear screen
- "\x1b[K": erase line to the left of cursor
- 127: backspace
- 13: enter

