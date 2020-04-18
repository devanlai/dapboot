/*
 * memcpy.c: Smaller memcpy implementation.
 */

#include <string.h>

// This makes GCC avoid replacing this with a call to builtin_memcpy
#pragma GCC optimize "Os"

void *memcpy(void *dst, const void *src, size_t len)
{
    char *d = (char *)dst;
    const char *s = (const char *)src;

    while(len--)
        *d++ = *s++;

    return dst;
}
