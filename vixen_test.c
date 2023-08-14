#include <stdarg.h>
#include <stdio.h>

#include "vixen.h"

// Writes the provided message to stderr and then
// exits.
void panicf(const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "panic: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}

void assert(int res, const char* reason, ...) {
    if (res) return;
    va_list args;

    fprintf(stderr, "failure: ");
    va_start(args, reason);
    vfprintf(stderr, reason, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(EXIT_FAILURE);
}

int main(void) {
    assert(1 != 2, "1 must not equal 2.");
}
