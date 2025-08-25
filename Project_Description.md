# General project rules 
- Do not modify this descripition document.

# Real Time Grep phase 1 

- For phase 1 of this project, we will create a simple terminal application in c.  When the user runs the application, a curses tui will be displayed with two  panes. A Small pane across the bottom of the screen will be the *Input Field*, and all of the space above the *Input Field* will be the *Output Field*.  The layout of the two fields should look like the following.


+-----------------------------------------------------------+
|file.txt:2: ----Grep pattern match----                     |
|file.txt:4: -----anether Grep pattern match---             |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|-----------------------------------------------------------|
|> Grep pattern                                             |
+-----------------------------------------------------------+

- When the user types a pattern into the *Input Field* then presses the `<Return>` key,  the program will fork a grep process using the pattern in the *Input Field*.  The output from the grep process will be routed to the *Output Field*.  For now, when there are more lines of output then fit in the *Output Field*,  discard the rest of the results.  In the next phase of development we will handle scrolling through results, but to start, lets just keep things simple. 

# Real Time Grep phase 2 
- The next phase of the project is to implement the Real Time component of the name. in this version, when the user types a pattern, the grep command shall be called as the user is typing.  The following rules shall be followed for resource management. 
    1. Only 1 grep process shall run at a time.
    2. Grep shall not be run if the *Input Field* is empty 
    3. Grep shall run if the user stops typeing for half a second.  If the user types while a grep process is run in the background,  the grep process shall be killed and a new one will not start until the user stops typing. 
