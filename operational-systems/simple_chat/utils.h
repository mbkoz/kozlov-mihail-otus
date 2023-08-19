#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

static inline void error(const char* const str) {
    fprintf(stderr, "error has occuried: %s, error message: %s\n", strerror(errno), str);
    exit(EXIT_FAILURE);
}
