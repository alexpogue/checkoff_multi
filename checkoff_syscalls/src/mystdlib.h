#ifndef MYSTDLIB_H
#define MYSTDLIB_H
#define EOF (-1)
#define NULL ((void*)0)

typedef unsigned long size_t;

unsigned short my_htons(unsigned short x);
void int_to_str(int, char*);
int my_strncmp(const char*, const char*, size_t);
long my_strlen(const char *str);
int my_puts(const char *str);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strncat(char *dest, const char *src, size_t n);

#endif
