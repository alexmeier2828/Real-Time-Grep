#include <stdio.h>

int run_line_list_tests();
int run_arguments_tests();

int main(int argc, char** argv) {
    printf("Running all tests...\n\n");
    
    int line_list_result = run_line_list_tests();
    printf("\n");
    int arguments_result = run_arguments_tests();
    
    int total_result = line_list_result + arguments_result;
    
    if (total_result == 0) {
        printf("\nAll tests passed!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return total_result;
}
