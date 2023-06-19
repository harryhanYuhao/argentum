# The design of the Editor

The editor shall have modes similar to that of vim.

## Cursor Control

Screen will show the approipriate portion of the buffer with auxillary informations; 
pressing key will modify the buffer accordingly, or change the way how the textbuffer is presented on screen. 

### CTP and CSP 

The cursor textbuffer position (CTP) is the position of the cursor in the textbuffer. 

The cursor textbuffer position is the base for cursor screen position (CSP). Textbuffer is edited based on CTP, and CTP may be changed accordingly. The cursor screen position is modified according to CTP. Finally, the cursor is renderred onto the screen with its CSP.

## Reading from Files

There shall be text buffer, storing the openned file. Many files may be openned at the same time, each with their correspounding buffer.

The buffer shall be stored in memory and in a cached filed. The cached file is constantly written into as a mean of crash migitation.

## Key Listeners

The editor shall enable multiple key listeners, i.e., it may differentiate `c` and `cc`.

The key listener shall be in a different thread. 

## Writing to Files

## Display

The display is row-based, i.e., each row of the text is rendered in the terminal, with their respective decorators and line numbers. 

### Warning System

There shall be displayed warning system, maybe implemented simply as a function taking in string.
