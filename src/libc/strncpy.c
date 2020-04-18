/*
 * strncpy.c: Smaller strncpy implementation.
 */

#include <string.h>

// This makes GCC avoid replacing this with a call to builtin_strncpy
#pragma GCC optimize "Os"

char *strncpy(char *dst, const char *src, size_t len)
{
    char *d = dst;
    while( len-- && '\0' != (*d++ = *src++) );
    return dst;
}
