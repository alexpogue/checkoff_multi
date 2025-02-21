#ifndef MYSTDLIB_H
#define MYSTDLIB_H
#define EOF (-1)
#define NULL ((void*)0)

typedef unsigned long size_t;
typedef long long ptrdiff_t;

unsigned short my_htons(unsigned short x);
char *int_to_str(int, char*);
void print_int(int);
int my_strncmp(const char*, const char*, size_t);
long my_strlen(const char *str);
size_t my_strnlen(const char *str, size_t maxlen);
int my_puts(const char *str);
char *my_strncpy(char *dest, const char *src, size_t n);
char *my_strncat(char *dest, const char *src, size_t n);
void *my_memchr(const void *str, int ch, size_t n);
char *my_strchr(const char *s, int c);
char *my_strstr(const char *s1, const char *s2);

#endif
