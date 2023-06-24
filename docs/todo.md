# TODO-LIST

## Urgent

- Set E.cursorTextbufPosX and E.cursorTextbufPosY to be int. They are not unsigned int.
- Implement cursor position and textbuf position global variables and refactor functions accordingly.
- Cursor shall not be moved to area where inputs are not allowed, i.e., before the first line of the text, in the padding area of line number. 
- Modal Editor Like vim
- Improve move cursor functionality
- Set proper restriction for input to avoid seg fault.

## Good to Have

- Replace reading file method with syscall
- Make `struct keyValue V` a const struct.

## Finshed Upgrades

- Create appropriate padding (line number, etc)
