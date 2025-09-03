#include <stdio.h>

int run_line_list_tests();

int main(int argc, char** argv) {
    printf("Running all tests...\n\n");
    
    int result = run_line_list_tests();
    
    if (result == 0) {
        printf("\nAll tests passed!\n");
    } else {
        printf("\nSome tests failed!\n");
    }
    
    return result;
}
