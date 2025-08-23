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


