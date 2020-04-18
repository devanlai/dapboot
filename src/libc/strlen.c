/*
 * strlen.c: Smaller strlen implementation.
 */

#include <string.h>

// This makes GCC avoid replacing this with a call to builtin_strlen
#pragma GCC optimize "Os"

size_t strlen(const char *str)
{
    const char *start = str + 1;
    while(*str++);
    return str - start;
}
