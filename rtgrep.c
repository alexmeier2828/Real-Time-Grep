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

#define MAX_PATTERN_LEN 256
#define MAX_OUTPUT_LINES 1000
#define MAX_LINE_LEN 512
#define TYPING_DELAY_MS 500

typedef struct {
    WINDOW *output_win;
    WINDOW *input_win;
    int output_height;
    int input_height;
    int width;
} ui_context_t;

typedef struct {
    char lines[MAX_OUTPUT_LINES][MAX_LINE_LEN];
    int count;
    int display_start;
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
int handle_input(ui_context_t *ui, char *pattern, grep_state_t *grep_state);
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
        draw_ui(&ui, pattern, &output);
        
        if (should_execute_grep(pattern, &grep_state)) {
            execute_grep(pattern, &output, &grep_state);
        }
        
        if (handle_input(&ui, pattern, &grep_state) == -1) {
            break;
        }
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
    
    getmaxyx(stdscr, ui->output_height, ui->width);
    ui->input_height = 3;
    ui->output_height -= ui->input_height;
    
    ui->output_win = newwin(ui->output_height, ui->width, 0, 0);
    ui->input_win = newwin(ui->input_height, ui->width, ui->output_height, 0);
    
    scrollok(ui->output_win, TRUE);
    keypad(ui->input_win, TRUE);
    nodelay(ui->input_win, TRUE);
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
    werase(ui->output_win);
    werase(ui->input_win);
    
    box(ui->output_win, 0, 0);
    box(ui->input_win, 0, 0);
    
    int display_lines = ui->output_height - 2;
    int start_line = (output->count > display_lines) ? output->count - display_lines : 0;
    
    for (int i = 0; i < output->count && i < display_lines; i++) {
        int line_idx = start_line + i;
        mvwprintw(ui->output_win, i + 1, 1, "%.80s", output->lines[line_idx]);
    }
    
    mvwprintw(ui->input_win, 1, 1, "> %s", pattern);
    
    wrefresh(ui->output_win);
    wrefresh(ui->input_win);
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
    
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        return;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }
    
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);
        
        execlp("grep", "grep", "-rn", pattern, ".", NULL);
        exit(1);
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
 * Handles keyboard input from the user in the input field
 * Processes character input, backspace, Enter key, and ESC key
 * Returns -1 when user wants to exit (ESC), 0 otherwise
 * Triggers grep execution when Enter is pressed
 */
int handle_input(ui_context_t *ui, char *pattern, grep_state_t *grep_state) {
    int ch = wgetch(ui->input_win);
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