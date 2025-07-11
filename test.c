#include <stdio.h>

int main() {
    printf("%d\n", __builtin_popcountll(0x400800000)); // Example output
    return 0;
}