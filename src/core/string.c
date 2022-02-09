#include "core/string.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct string* string_new(const char* initial)
{
    struct string* new_str = malloc(sizeof(struct string));

    string_init(new_str, initial);

    return new_str;
}

void string_free(struct string* str)
{
    free(str);
}

void string_init(struct string* str, const char* initial)
{
    str->size = strlen(initial);
    str->buffer = malloc(str->size + 1);

    snprintf(str->buffer, str->size + 1, "%s", initial);
}

void string_uninit(struct string* str)
{
    str->size = 0;
    free(str->buffer);
}

int string_size(struct string* str)
{
    return str->size;
}

void string_printf(struct string* str)
{
    printf(str->buffer);
}

int string_rfindi(struct string* str, const char needle, size_t start)
{
    const char* haystack = str->buffer;

    if(strlen(haystack) < start)
    {
        return -1;
    }

    const char* curr = haystack + start;

    do
    {
        if(*curr == needle)
        {
            return (int)(curr - haystack) + 1;
        }

        --curr;
    }
    while(curr >= haystack);

    return -1;
}
