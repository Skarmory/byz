#ifndef BYZ_STRING_H
#define BYZ_STRING_H

struct string;

struct string* string_new(const char* initial);
void           string_free(struct string* str);
int            string_size(struct string* str);

// Helpful functions
void           string_printf(struct string* str);

#endif
