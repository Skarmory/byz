#ifndef BYZ_STRING_H
#define BYZ_STRING_H

#include <stddef.h>

struct string
{
    char* buffer;
    int   size;
};

struct string* string_new(const char* initial);
void           string_free(struct string* str);
void           string_init(struct string* str, const char* initial);
void           string_uninit(struct string* str);
int            string_size(struct string* str);

// Helpful functions
void           string_printf(struct string* str);
int            string_rfindi(struct string* str, const char needle, size_t start);
void           string_format(struct string* str, const char* format, ...);

#endif
