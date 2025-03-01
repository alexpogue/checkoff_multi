#include "mystdlib.h"
#include "syscall.h"

#define STDOUT_FILENO 1

#define INT_MIN -2147483648

#ifdef UNUSED
char *int_to_str(int value, char *str) {
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
    return str;
}

void print_int(const int i) {
  char buf[12];
  my_puts(int_to_str(i, buf));
}
#endif

unsigned short my_htons(unsigned short x) {
      return (x << 8) | (x >> 8);
}

int my_strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0 || (s1 == NULL && s2 == NULL)) return 0; // no chars, or NULL==NULL
  if (s1 == NULL) return -1; // treat NULL smaller than string
  if (s2 == NULL) return 1;
  n -= 1; // read all but one character
  // iterate s1/s2 to second to last char, or where a char is null, or where chars not equal
  while (n-- && *s1 && *s2 && *s1 == *s2) {
    s1++;
    s2++;
  }

  // return final comparison - either '\0' with '\0', '\0' with letter,
  //                           first non-equal chars, or two last chars
  // cast in case char is signed, as some platforms/compilers do
  return (unsigned char)*s1 - (unsigned char)*s2;
}

long my_strlen(const char *str) {
  if (!str)
    return 0;
  long i = 0;
  for (; str[i] != '\0'; ++i) {}
  return i;
}

size_t my_strnlen(const char *str, size_t maxlen) {
  if (!str)
    return 0;
  size_t i = 0;
  for (; str[i] != '\0' && i < maxlen; ++i) {}
  return i;
}

#ifdef UNUSED
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
#endif

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

int my_putchar(const char c) {
    long ret = write(STDOUT_FILENO, &c, 1);
    if (ret < 0)
      return EOF;
    return 0;
}

int my_puts(const char *str) {
    long ret = write(STDOUT_FILENO, str, my_strlen(str));
    long ret2 = write(STDOUT_FILENO, "\n", 1);
    if (ret < 0 || ret2 < 0)
      return EOF;
    return 0;
}

#ifdef UNUSED
void *my_memchr(const void *mem, int ch, size_t n) {
  if (!mem) return NULL;
  unsigned char *ptr = (unsigned char*)mem;
  for (; n--; ptr++) {
    if (*ptr == (unsigned char)ch)
      return ptr;
  }
  return NULL;
}
#endif

char *my_strchr(const char *str, int ch) {
  if (!str) return NULL;
  for (; *str != '\0'; str++) {
    if (*str == (char)ch) {
      return (char*)str;
    }
  }
  return NULL;
}

char *my_strstr(const char *s1, const char *s2) {
  if (!s1 || !s2) return NULL;

  size_t s2_len = my_strlen(s2);
  if (s2_len == 0) return (char*)s1; // if needle empty string, found
  if (s2_len == 1) return my_strchr(s1, s2[0]);

  // if needle is bigger than haystack, never found
  if (s2_len > my_strnlen(s1, s2_len + 1)) return NULL;

  // iterate haystack until needle no longer fits in the leftover chars
  // e.g. haystack: "world", needle: "rld", iterate haystack to "r", exit at "l"
  for (;*(s1 + s2_len - 1) != '\0'; s1++) {
    const char *s2_ptr = s2;
    // modify s1 directly, then reset s1 using s2's offset, no need for s1_ptr
    while (*s2_ptr != '\0' && *s1 == *s2_ptr) {
      s1++;
      s2_ptr++;
    }
    if (*s2_ptr == '\0') {
      // all s2 chars matched, found! Rewind s1 to same place loop block began
      return (char*)(s1 - (s2_ptr - s2));
    }
    s1 -= (s2_ptr - s2); //rewind s1 for next iteration
  }
  return NULL;
}

#ifdef UNUSED
char *my_strtok(char *str, const char *delims) {
  if (!delims) return NULL;
  static char *internal_str;
  if (str) {
    internal_str = str;
  }
  if (!internal_str) return NULL;

  // Skip leading delimiters
  while (*internal_str != '\0' && my_strchr(delims, *internal_str)) {
    internal_str++;
  }
  if (*internal_str == '\0') {
    // read all delimiters or empty string and reached end
    return NULL;
  }
  char *start = internal_str;
  if (!internal_str) return NULL;
  while (*internal_str != '\0' && !my_strchr(delims, *internal_str)) {
    ++internal_str;
  }
  if (*internal_str != '\0') {
    *internal_str = '\0'; // null terminate token
    ++internal_str; // move to next token for next str_tok call
  }
  return start;
}
#endif
