#include <stdio.h>

int main() {
    int pass = 0;
    printf("Initial pass: %d\n", pass);
    if (1) {
        printf("Condition true, about to pass++\n");
        pass++;
        printf("After pass++, pass: %d\n", pass);
    }
    if (1) {
        printf("Second condition true, about to pass++\n");
        pass++;
        printf("After second pass++, pass: %d\n", pass);
    }
    printf("Final pass: %d\n", pass);
    return 0;
}
