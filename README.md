# rtgrep - Real-Time Interactive Grep

A real-time interactive grep utility that provides a two-pane terminal interface for searching through files. As you type search patterns, rtgrep displays matching results instantly, making it ideal for quickly exploring and searching through codebases and text files.  This project was partially inspired by the neovim's Telescope, which has a "live grep" mode that displays grep results as you type.  I wanted something similar that was a standalone utility that could be integrated into other things (see rtgrep.vim for an example of integrating into traditional vim).

This project is still highly a work in progress and is mostly intended for my own use, but I've included a license just in case. 

## Features

- **Real-time search**: Results appear as you type (with 100ms delay)
- **Two-pane interface**: Output pane for results, input pane for patterns
- **Resource efficient**: Only one grep process runs at a time
- **Customizable grep command**: Use grep, ripgrep, ag, or any compatible tool
- **Color output preserved**: Maintains grep's color highlighting
- **Results preservation**: All results printed to stdout on exit

## Installation

### Build from source

```bash
make
```

### Install system-wide

```bash
make install
```

This installs the binary to `/usr/local/bin/rtgrep` and the man page to `/usr/local/share/man/man1/rtgrep.1`.

### Uninstall

```bash
make uninstall
```

## Usage

### Interactive Mode

Start rtgrep without arguments for interactive searching:

```bash
rtgrep
```

### With Initial Pattern

Start with a search pattern:

```bash
rtgrep "function.*main"
```

### Custom Grep Command

Use ripgrep instead of grep:

```bash
rtgrep -g "rg --color=always -n" "TODO"
```

Use ag (the silver searcher):

```bash
rtgrep -g "ag --color" "pattern"
```

## Interface

```
+-----------------------------------------------------------+
|file.txt:2: ----Grep pattern match----                     |
|file.txt:4: -----another Grep pattern match---             |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|                                                           |
|-----------------------------------------------------------|
|> Grep pattern                                             |
+-----------------------------------------------------------+
```

## Keyboard Controls

- **Type**: Add characters to search pattern
- **Backspace**: Delete last character
- **Enter/Escape**: Exit and print all results to stdout

## Command Line Options

- `-g COMMAND`: Use custom grep command (default: "grep -rn --color=always")
- `-h, --help`: Display help information

## Examples

Search for function definitions:
```bash
rtgrep "int main"
```

Find TODOs using ripgrep:
```bash
rtgrep -g "rg --color=always -n" "TODO"
```

Search and save results:
```bash
rtgrep "\.h:" > header_files.txt
```

## Development

### Build and Test

```bash
# Build the project
make

# Run tests
make test

# Clean build artifacts
make clean
```

### Project Structure

```
├── src/
│   ├── rtgrep.c          # Main application logic
│   ├── line_list.c       # Dynamic line storage
│   ├── line_list.h
│   ├── arguments.c       # Command line argument parsing
│   ├── arguments.h
│   └── ansi.h           # ANSI escape codes for UI
├── test/                 # Unit tests
├── man/
│   └── rtgrep.1         # Manual page
├── documentation/
│   └── Project_Description.md
└── Makefile
```

## Requirements

- **C compiler** (gcc recommended)
- **ncurses library** (`libncurses-dev` on Ubuntu/Debian)
- **POSIX-compliant system** (Linux, macOS, BSD)

## Technical Details

- **Language**: C (C99 standard)
- **Dependencies**: ncurses, POSIX system calls
- **Search behavior**: Recursive search through current directory
- **Resource limits**: 
  - Max pattern length: 256 characters
  - Max line length: 512 characters  
  - Max stored results: 1000 lines
- **Timing**: 100ms delay after last keypress before executing search

## License

See LICENSE file for details.
