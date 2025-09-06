#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../arguments.h"
#include "test_utils.h"

void test_basic_pattern_only() {
    char* argv[] = {"rtgrep", "test_pattern"};
    int argc = 2;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, "test_pattern") == 0, "pattern matches expected value");
    test_assert(args->grep_command == NULL, "grep_command is NULL when not specified");
    
    deallocate_arguments(&args);
    test_assert(args == NULL, "deallocate_arguments sets pointer to NULL");
}

void test_pattern_with_grep_command() {
    char* argv[] = {"rtgrep", "test_pattern", "-g", "rg"};
    int argc = 4;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, "test_pattern") == 0, "pattern matches expected value");
    test_assert(args->grep_command != NULL, "grep_command is set");
    test_assert(strcmp(args->grep_command, "rg") == 0, "grep_command matches expected value");
    
    deallocate_arguments(&args);
}

void test_grep_command_before_pattern() {
    char* argv[] = {"rtgrep", "-g", "grep", "search_term"};
    int argc = 4;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, "search_term") == 0, "pattern matches expected value");
    test_assert(args->grep_command != NULL, "grep_command is set");
    test_assert(strcmp(args->grep_command, "grep") == 0, "grep_command matches expected value");
    
    deallocate_arguments(&args);
}

void test_no_pattern() {
    char* argv[] = {"rtgrep", "-g", "rg"};
    int argc = 3;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern == NULL, "pattern is NULL when no non-option arguments");
    test_assert(args->grep_command != NULL, "grep_command is set");
    test_assert(strcmp(args->grep_command, "rg") == 0, "grep_command matches expected value");
    
    deallocate_arguments(&args);
}

void test_empty_strings() {
    char* argv[] = {"rtgrep", "", "-g", ""};
    int argc = 4;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set for empty string");
    test_assert(strcmp(args->pattern, "") == 0, "pattern handles empty string");
    test_assert(args->grep_command != NULL, "grep_command is set for empty string");
    test_assert(strcmp(args->grep_command, "") == 0, "grep_command handles empty string");
    
    deallocate_arguments(&args);
}

void test_complex_strings() {
    char* argv[] = {"rtgrep", ".*complex[0-9]+pattern.*", "-g", "/usr/bin/grep -E"};
    int argc = 4;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, ".*complex[0-9]+pattern.*") == 0, "pattern handles complex regex");
    test_assert(args->grep_command != NULL, "grep_command is set");
    test_assert(strcmp(args->grep_command, "/usr/bin/grep -E") == 0, "grep_command handles path with args");
    
    deallocate_arguments(&args);
}

void test_multiple_non_option_args() {
    char* argv[] = {"rtgrep", "first_pattern", "second_arg", "-g", "rg"};
    int argc = 5;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args != NULL, "get_cli_arguments returns non-NULL");
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, "first_pattern") == 0, "pattern uses first non-option argument");
    test_assert(args->grep_command != NULL, "grep_command is set");
    test_assert(strcmp(args->grep_command, "rg") == 0, "grep_command matches expected value");
    
    deallocate_arguments(&args);
}

void test_pattern_only() {
    char* argv[] = {"rtgrep", "first_pattern"};
    int argc = 2;
    
    arguments_t* args = get_cli_arguments(argc, argv);
    
    test_assert(args->pattern != NULL, "pattern is set");
    test_assert(strcmp(args->pattern, "first_pattern") == 0, "pattern uses first non-option argument");
    test_assert(args->grep_command == NULL, "grep_command not set");
    
    deallocate_arguments(&args);

}

int run_arguments_tests() {
    reset_test_counters();
    printf("Running arguments tests...\n");
    
    test_basic_pattern_only();
    test_pattern_with_grep_command();
    test_grep_command_before_pattern();
    test_no_pattern();
    test_empty_strings();
    test_complex_strings();
    test_multiple_non_option_args();
    test_pattern_only();
    
    printf("\nArguments tests completed: %d/%d passed\n", test_passed, test_count);
    return (test_passed == test_count) ? 0 : 1;
}

