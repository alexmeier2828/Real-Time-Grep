#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "ansi.h"

#define MAX_PATTERN_LEN 256
#define MAX_OUTPUT_LINES 1000
#define MAX_LINE_LEN 512
#define TYPING_DELAY_MS 250


typedef struct {
    int input_height;
    int width;
    int height;
    char last_pattern[MAX_PATTERN_LEN];
    int separator_drawn;
    int input_needs_refresh;
} ui_context_t;

typedef struct {
    char lines[MAX_OUTPUT_LINES][MAX_LINE_LEN];
    int count;
    int display_start;
    int last_displayed_count;
    int needs_full_redraw;
} output_buffer_t;

typedef struct {
    pid_t current_grep_pid;
    struct timeval last_keypress_time;
    int timer_active;
} grep_state_t;

void init_ui(ui_context_t *ui);
void cleanup_ui(void);
void draw_ui(ui_context_t *ui, const char *pattern, output_buffer_t *output);
void execute_grep(const char *pattern, output_buffer_t *output, grep_state_t *grep_state);
void grep_process(int pipefd[2], const char pattern[]);
int handle_input(char *pattern, grep_state_t *grep_state);
void kill_current_grep(grep_state_t *grep_state);
int should_execute_grep(const char *pattern, grep_state_t *grep_state);
void update_keypress_time(grep_state_t *grep_state);

/**
 * Main function - initializes the application and runs the main event loop
 * Handles the overall program flow including UI initialization, input processing,
 * and cleanup when the program exits.
 */
int main(void) {
    ui_context_t ui;
    output_buffer_t output = {0};
    char pattern[MAX_PATTERN_LEN] = "";
    grep_state_t grep_state = {0};
    
    init_ui(&ui);
    
    while (1) {
        
        if (should_execute_grep(pattern, &grep_state)) {
            execute_grep(pattern, &output, &grep_state);
        }
        
        if (handle_input(pattern, &grep_state) == -1) {
            break;
        }
        
        draw_ui(&ui, pattern, &output);
    }
    
    kill_current_grep(&grep_state);
    cleanup_ui();
    return 0;
}

/**
 * Initializes the ncurses UI and creates the two-pane layout
 * Sets up the output window (large pane) and input window (bottom pane)
 * Configures ncurses settings for proper input handling and display
 */
void init_ui(ui_context_t *ui) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    
    getmaxyx(stdscr, ui->height, ui->width);
    ui->input_height = 3;
    ui->last_pattern[0] = '\0';
    ui->last_pattern[1] = '?'; // Ensure it won't match empty pattern initially
    ui->separator_drawn = 0;
    ui->input_needs_refresh = 1;
}

/**
 * Cleans up ncurses resources and restores terminal state
 * Should be called before program exit to properly restore the terminal
 */
void cleanup_ui(void) {
    endwin();
}

/**
 * Draws the complete UI including both panes with current data
 * Displays grep results in the output pane and the current pattern in the input pane
 * Handles display of results that exceed the output pane size by showing most recent lines
 */
void draw_ui(ui_context_t *ui, const char *pattern, output_buffer_t *output) {
    int display_lines = ui->height - ui->input_height - 1;
    int start_line = (output->count > display_lines) ? output->count - display_lines : 0;
    
    if (output->needs_full_redraw) {
        printf(ANSI_CLEAR_SCREEN);
        
        for (int i = 0; i < output->count && i < display_lines; i++) {
            int line_idx = start_line + i;
            printf(ANSI_GOTO_POS "%s\n", i + 1, 1, output->lines[line_idx]);
        }
        
        output->needs_full_redraw = 0;
        output->last_displayed_count = output->count;
        ui->separator_drawn = 0;
        ui->input_needs_refresh = 1;
    } else if (output->count > output->last_displayed_count) {
        int lines_to_add = output->count - output->last_displayed_count;
        
        if (lines_to_add >= display_lines) {
            printf(ANSI_GOTO_POS, 1, 1);
            for (int i = 0; i < display_lines; i++) {
                int line_idx = start_line + i;
                printf(ANSI_CLEAR_LINE "%s\n", output->lines[line_idx]);
            }
        } else {
            for (int i = output->last_displayed_count; i < output->count; i++) {
                if (i - start_line >= 0 && i - start_line < display_lines) {
                    int display_row = (i - start_line) + 1;
                    printf(ANSI_GOTO_POS "%s\n", display_row, 1, output->lines[i]);
                }
            }
        }
        
        output->last_displayed_count = output->count;
    }
    
    if (!ui->separator_drawn) {
        printf(ANSI_GOTO_POS, ui->height - 2, 1);
        printf(ANSI_CLEAR_LINE);
        for (int i = 0; i < ui->width; i++) {
            printf(ANSI_HORIZONTAL_LINE);
        }
        ui->separator_drawn = 1;
    }
    
    if (strcmp(ui->last_pattern, pattern) != 0 || ui->input_needs_refresh) {
        strncpy(ui->last_pattern, pattern, MAX_PATTERN_LEN - 1);
        ui->last_pattern[MAX_PATTERN_LEN - 1] = '\0';
        ui->input_needs_refresh = 0;
        
        // Draw input buffer with ANSI escape sequences only when needed
        // Use only 2 lines: separator and input line
        printf(ANSI_GOTO_POS, ui->height - 1, 1);
        printf(ANSI_CLEAR_LINE);
        printf("| > %s", pattern);
        // Fill remaining space 
        int used_chars = 4 + strlen(pattern); // "| > " + pattern
        for (int i = used_chars; i < ui->width - 1; i++) {
            printf(" ");
        }
        printf("|");
    }
    
    fflush(stdout);
}

/**
 * Executes grep command with the given pattern and captures output
 * Forks a grep process, sets up pipes for communication, and reads the results
 * Stores grep output in the output buffer, discarding excess results if needed
 */
void execute_grep(const char *pattern, output_buffer_t *output, grep_state_t *grep_state) {
    if (strlen(pattern) == 0) {
        return;
    }
    
    kill_current_grep(grep_state);
    
    output->count = 0;
    output->needs_full_redraw = 1;
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        // Fork failed 
        close(pipefd[0]);
        close(pipefd[1]);
        printf("Failed to fork process!");
        return;
    }
    
    if (pid == 0) {
        grep_process(pipefd, pattern);
    } else {
        grep_state->current_grep_pid = pid;
        close(pipefd[1]);
        
        char buffer[MAX_LINE_LEN];
        char line[MAX_LINE_LEN];
        int line_pos = 0;
        ssize_t bytes_read;
        
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            
            for (int i = 0; i < bytes_read && output->count < MAX_OUTPUT_LINES; i++) {
                if (buffer[i] == '\n') {
                    line[line_pos] = '\0';
                    strncpy(output->lines[output->count], line, MAX_LINE_LEN - 1);
                    output->lines[output->count][MAX_LINE_LEN - 1] = '\0';
                    output->count++;
                    line_pos = 0;
                } else if (line_pos < MAX_LINE_LEN - 1) {
                    line[line_pos++] = buffer[i];
                }
            }
        }
        
        close(pipefd[0]);
        
        waitpid(pid, NULL, 0);
        grep_state->current_grep_pid = 0;
    }
}

/**
 * This function represents the child process that will run the grep command specified by the user
 */
void grep_process(int pipefd[2], const char pattern[]){
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO); // duplicate file descriptor of stdout and err 
    dup2(pipefd[1], STDERR_FILENO); // to the input side of the pipe
    close(pipefd[1]);
    
    execlp("grep", "grep", "-rn", "--color=always", pattern, ".", NULL);
    exit(1);
}

/**
 * Handles keyboard input from the user in the input field
 * Processes character input, backspace, Enter key, and ESC key
 * Returns -1 when user wants to exit (ESC), 0 otherwise
 * Triggers grep execution when Enter is pressed
 */
int handle_input(char *pattern, grep_state_t *grep_state) {
    int ch = getch();
    int pattern_len = strlen(pattern);
    int pattern_changed = 0;
    
    if (ch == ERR) {
        return 0;
    }
    
    switch (ch) {
        case 27: // ESC key
            return -1;
            
        case KEY_ENTER:
        case '\n':
        case '\r':
            break;
            
        case KEY_BACKSPACE:
        case 127:
        case '\b':
            if (pattern_len > 0) {
                pattern[pattern_len - 1] = '\0';
                pattern_changed = 1;
            }
            break;
            
        default:
            if (ch >= 32 && ch <= 126 && pattern_len < MAX_PATTERN_LEN - 1) {
                pattern[pattern_len] = ch;
                pattern[pattern_len + 1] = '\0';
                pattern_changed = 1;
            }
            break;
    }
    
    if (pattern_changed) {
        kill_current_grep(grep_state);
        update_keypress_time(grep_state);
    }
    
    return 0;
}

void kill_current_grep(grep_state_t *grep_state) {
    if (grep_state->current_grep_pid > 0) {
        kill(grep_state->current_grep_pid, SIGTERM);
        waitpid(grep_state->current_grep_pid, NULL, WNOHANG);
        grep_state->current_grep_pid = 0;
    }
}

void update_keypress_time(grep_state_t *grep_state) {
    gettimeofday(&grep_state->last_keypress_time, NULL);
    grep_state->timer_active = 1;
}

int should_execute_grep(const char *pattern, grep_state_t *grep_state) {
    if (strlen(pattern) == 0) {
        return 0;
    }
    
    if (!grep_state->timer_active) {
        return 0;
    }
    
    if (grep_state->current_grep_pid > 0) {
        return 0;
    }
    
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    
    long elapsed_ms = (current_time.tv_sec - grep_state->last_keypress_time.tv_sec) * 1000 +
                      (current_time.tv_usec - grep_state->last_keypress_time.tv_usec) / 1000;
    
    if (elapsed_ms >= TYPING_DELAY_MS) {
        grep_state->timer_active = 0;
        return 1;
    }
    
    return 0;
}

