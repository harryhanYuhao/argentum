# The design of the Editor

The editor shall have modes similar to that of vim.

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
