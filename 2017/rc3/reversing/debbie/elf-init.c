#include <stdio.h>
#include <stdlib.h>

void *giveMeLiberty() {
    void *ptr = calloc(8000, sizeof(char));
    printf("PTR = %p\n", ptr);
    return ptr;
}


void giveMeDeath(void *ptr) {
    printf("ptr = %p\n", ptr);
    free(ptr);
}

int liberty() {
    puts("FREEEEEEEEEEE");
    return 0;
}
