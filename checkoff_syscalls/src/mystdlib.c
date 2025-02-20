#include "mystdlib.h"
#include "syscall.h"

#define STDOUT_FILENO 1

#define INT_MIN -2147483648

void int_to_str(int value, char *str) {
    int i = 0;
    if (value == INT_MIN) {
      str[0] = '-';
      str[1] = '2';
      value = 147483648;
      i = 2;
    }
    int neg = 0;
    if (value < 0) {
      neg = 1;
      value *= -1;
    }
    char buffer[12];
    int pos = 10;
    buffer[11] = '\0';
    if (value == 0) {
        buffer[pos--] = '0';
    } else {
        while (value != 0) {
            buffer[pos--] = '0' + (value % 10);
            value /= 10;
        }
    }
    if (neg) {
      str[i++] = '-';
    }
    while (buffer[pos + 1] != '\0') {
        str[i++] = buffer[++pos];
    }
    str[i] = '\0';
}

unsigned short my_htons(unsigned short x) {
      return (x << 8) | (x >> 8);
}

int my_strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0 || (s1 == NULL && s2 == NULL)) return 0; // no chars, or NULL==NULL
  if (s1 == NULL) return -1; // treat NULL smaller than string
  if (s2 == NULL) return 1;
  n -= 1; // read all but one character
  // iterate s1/s2 to second to last char, or where a char is null, or where chars not equal
  while (n-- && *s1 && *s2 && *(s1++) == *(s2++)) {}
  // return final comparison - either '\0' with '\0', '\0' with letter, or two last letters
  // cast in case char is signed, as some platforms/compilers could
  return (unsigned char)*s1 - (unsigned char)*s2;
}

long my_strlen(const char *str) {
  if (!str)
    return 0;
  long i = 0;
  for (; str[i] != '\0'; ++i) {}
  return i;
}

char *my_strncat(char *dest, const char *src, size_t n) {
  if (!dest || !src)
    return dest;
  char *dest_ptr = dest;
  for (; *dest_ptr != '\0'; dest_ptr++) {} // bring dest_ptr to the null terminator of dest
  for (int i = 0; i < n && src[i] != '\0'; i++, dest_ptr++)
    *dest_ptr = src[i]; // copy until n or src null terminator
  *dest_ptr = '\0'; // add null terminator at n or if we found src null terminator earlier
  return dest;
}

char *my_strncpy(char *dest, const char *src, size_t n) {
  if (!dest || !src)
    return dest; // NULL if !dest, and also should return dest without changes if src is NULL
  int i = 0;
  for (; i < n && src[i] != '\0'; i++)
    // copy characters
    dest[i] = src[i];
  for (;i < n; i++)
    // pad with nulls, per documentation
    dest[i] = '\0';
  return dest;
}

int my_puts(const char *str) {
    long ret = write(STDOUT_FILENO, str, my_strlen(str));
    long ret2 = write(STDOUT_FILENO, "\n", 1);
    if (ret < 0 || ret2 < 0)
      return EOF;
    return 0;
}
