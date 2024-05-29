#include "xmalloc.h"
#include <stdio.h>

void *malloc_or_exit(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "%s:%d: malloc(%zu) failed\n", file, line, size);
        exit(EXIT_FAILURE);
    }else{
        return ptr;
    }
}