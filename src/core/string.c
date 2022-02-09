#include "core/string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct string
{
    char* buffer;
    int   size;
};

struct string* string_new(const char* initial)
{
    struct string* new_str = malloc(sizeof(struct string));

    new_str->size = strlen(initial);
    new_str->buffer = malloc(new_str->size + 1);
    
    snprintf(new_str->buffer, new_str->size, "%s", initial);

    return new_str;
}

void string_free(struct string* str)
{
    free(str->buffer);
    free(str);
}

int string_size(struct string* str)
{
    return str->size;
}

void string_printf(struct string* str)
{
    printf(str->buffer);
}
