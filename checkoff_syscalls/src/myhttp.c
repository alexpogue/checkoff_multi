#include "myhttp.h"

/* //unused
void deep_copy_request(http_request_t *dest, http_request_t *src) {
  dest->method = src->method;
}
*/

char *get_first_from_list(const char *haystack, const char **needles, size_t num_needles, size_t *which_needle_out) {
  if (!haystack || !needles || num_needles == 0) return NULL;
  for (size_t i = 0; i < num_needles; i++) {
    if (!needles[i]) continue;
    char *pos = my_strstr(haystack, needles[i]);
    if (pos) {
      if (which_needle_out)
        *which_needle_out = i;
      return pos;
    }
  }
  return NULL;
}

char *get_next_newline(const char *buf, unsigned char *out_num_chars_in_newline) {
  if (!buf) return NULL;
  const char *newlines[] = {"\r\n", "\n", "\r"};
  size_t which_one;
  char *match = get_first_from_list(buf, newlines, (sizeof(newlines) / sizeof(newlines[0])), &which_one);
  if (!match) {
    return NULL;
  }
  else {
    if (out_num_chars_in_newline)
      *out_num_chars_in_newline = my_strlen(newlines[which_one]);
    return match;
  }
}

// Extract the string from the current location until the separator, eat the
// separator, and return the char* at the first character after the separator.
// If dest is NULL, don't use it, but still chomp and fill dest_size parameter
char *chomp_field(char *buf, char *separator, char *dest, size_t *dest_size) {
  if (!buf || dest_size == 0 || !separator) return NULL;
  char *separator_loc = my_strstr(buf, separator);
  if (!separator_loc) return NULL;
  size_t field_len = separator_loc - buf;
  if (dest) {
    if (*dest_size < field_len + 1) {
      my_puts("Error: dest_size is not big enough to fit field");
      return NULL;
    }
    my_strncpy(dest, buf, field_len);
    dest[field_len] = '\0';
  } else {
    if (dest_size)
      *dest_size = separator_loc - buf + 1;
  }
  return separator_loc + my_strlen(separator);
}
/* TODO: continue
 *
char *read_headers(char *buf, char **headers, size_t *num_headers, size_t *header_sizes) {
  if (!buf || !headers) return NULL;

  size_t num_chars_of_headers;
  char *body_start = chomp_field(buf, "\r\n\r\n", NULL, &num_chars_of_headers);
  if (!body_start) {
    my_puts("likely there is no body");
    num_chars_of_headers = my_strlen(buf);
  }
  char *cur = buf;

  chomp_field(cur, ": "

}
*/

// if char* outputs are NULL, do "dry run" and fill size parameters
int parse_request_fill_sizes(char *request_str,
                  char *method, size_t *method_size,
                  char *path, size_t *path_size,
                  char *protocol, size_t *protocol_size,
                  char **headers, size_t *num_headers,
                  size_t *header_sizes) {
  if (!request_str) {
    return -1;
  }

  //http_request_t tmp_request;
  char *cur = request_str;

  cur = chomp_field(cur, " ", method, method_size);
  if (!cur) return -1;

  cur = chomp_field(cur, " ", path, path_size);
  if (!cur) return -1;

  cur = chomp_field(cur, "\r\n", protocol, protocol_size);
  if (!cur) return -1;

  //read_headers(cur, headers, num_headers, header_sizes);

  return 0;
}

// Convenience function so the user doesn't have to pass addresses
int parse_request(char *request_str,
                  char *method, size_t method_size,
                  char *path, size_t path_size,
                  char *protocol, size_t protocol_size) {
  return parse_request_fill_sizes(request_str,
                                  method, &method_size,
                                  path, &path_size,
                                  protocol, &protocol_size,
                                  NULL, NULL, NULL);
}

/* // unused
char *http_method_to_string(char *dest, http_method_t method, size_t n) {
  char *ret;
  if (method == HTTP_METHOD_GET)
    ret = my_strncpy(dest, "GET", n);
  else if (method == HTTP_METHOD_POST)
    ret = my_strncpy(dest, "POST", n);
  else
    ret = NULL;
  return ret;
}
*/
