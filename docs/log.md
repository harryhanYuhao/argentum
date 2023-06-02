# Coding Logs

## 2023

### May 21

Successful implementation of function `textbufDeleteChar` and `textbufInputChar`. These two functions rely heavily on dynamic memory management and are prone to 'off-by-one' error. Anormalties still remains mainly because I have hard coded the "paddings" for showing line numbers. 


### May 25

Created the struct `key` for keylistening. The modifiers of many declarations are justed to unsigned or const where appropriate.

### May 27

Refactor, create new file `editor.h`, `editor.c`.

Fix the that that view mode starting at the third line. 

### May 28

Replace some `sprintf()` with `snprintf()` as the former may not be memory safe.

Found the source of the bug that the view mode starting at the third line. This bug is due to line 182 of `editor.c`: the number of the lines input into `abptr` was more than the height of the screen.

## May 30

Added function `textbufNewLineAbove()` 

Change the mechanism of `disableRAWMode()`. Before change, this function will reset the terminal to the state when the program starts. I change it so that this function always sets the terminal to normal mode.  

## May 31 

Fixed bugs for function `textbufNewLineAbove()`. Rename it to `textbufEnter()`. 

`textbufEnter(int x, int y)` now acts like `enter` in a normal editor, where a new line is created below, holding the characters of the old line from the cursor, while the old line contains the character upto the cursor.


# June 1

Key listener refactored. 
Now each key read function `int editorReadKey(void)` will modify the global struct `struct key KEY` from which key processing function `int editorProcessKeyPress(void)`will read. 

Tomorrow delete line function shall be added and the editor would adequately perform the most fundamental function one may expect. 

# June 2

Delete line function successfully implemented and integrated with the backspace.

Create function `textbufDeleteLine` and `textbufDeleteLineBreak`, the former simply deletes the line while the latter deletes the line the append the rest of the line (if there is) to the previous line (just as a backspace used at the beginning of the line in a normal editor).
