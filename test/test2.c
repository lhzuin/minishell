#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s arg1 arg2 arg3 arg4\n", argv[0]);
        return 1;
    }
    printf("Arguments: %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);
    return 0;
}