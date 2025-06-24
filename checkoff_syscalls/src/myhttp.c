#include "myhttp.h"

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

// Extract the string from the current location until the separator, eat the
// separator, and return the char* at the first character after the separator.
// If dest is NULL, don't use it, but still chomp and fill dest_size parameter
char *chomp_field(char *buf, char *separator, char *dest, size_t *dest_size) {
  if (!buf || !separator) return NULL;
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
      *dest_size = field_len + 1;
  }
  return separator_loc + my_strlen(separator);
}

// precondition: buf is pointing to the character after the full header line + single newline chracters
// return: buf if more headers, start of body if no more headers
char *chomp_to_body_if_headers_complete(char *buf, char *newline_chars) {
  char *after_next_newline = chomp_field(buf, newline_chars, NULL, NULL); 
  if (!after_next_newline) {
    return NULL;
  }
  if (after_next_newline - my_strlen(newline_chars) == buf) {
    // if we saw immediate newline, we reached end of headers
    return after_next_newline;
  }
  return buf;
}

http_parse_token_t *bounds_check_lookup_token(http_parse_token_t *tokens, size_t i, size_t arr_len) {
  if (!tokens || i >= arr_len)
    return NULL;
  return tokens + i;
}

void fill_token(size_t start, size_t end, http_parse_token_type_t type, http_parse_token_t *token_out) {
  token_out->start = start;
  token_out->end = end;
  token_out->type = type;
}

char *chomp_and_get_token(char *buf, char *separator, size_t buf_offset, http_parse_token_type_t type, http_parse_token_t *token) {
  size_t size;
  char *cur = chomp_field(buf, separator, NULL, &size);
  size_t field_strlen = size - 1;
  if (token) {
    fill_token(buf_offset, buf_offset + field_strlen, type, token);
  }
  return cur;
}

char *read_headers(char *very_start_buf, char *buf, http_parse_token_t *tokens, size_t *token_offset, size_t num_tokens) {
  char *key_value_separator = ": ";
  char *newline = "\r\n";
  char *cur = buf;
  size_t token_count = 0;

  while (1) {
    char *body_check = chomp_to_body_if_headers_complete(cur, newline);
    if (!body_check) return NULL;
    int did_reach_body = body_check > cur;

    if (did_reach_body) {
      cur = body_check;
      break;
    }

    http_parse_token_t *token_to_fill = bounds_check_lookup_token(tokens, (*token_offset) + token_count, num_tokens);
    size_t full_offset = cur - very_start_buf;
    cur = chomp_and_get_token(cur, key_value_separator, full_offset, HTTP_HEADER_KEY, token_to_fill);
    ++token_count;

    token_to_fill = bounds_check_lookup_token(tokens, (*token_offset) + token_count, num_tokens);
    full_offset = cur - very_start_buf;
    cur = chomp_and_get_token(cur, newline, full_offset, HTTP_HEADER_VALUE, token_to_fill);
    ++token_count;
  }

  (*token_offset) += token_count;

  return cur;
}

char *get_body_token_if_exists(char *very_start_buf, char *buf, size_t request_str_len, http_parse_token_t *tokens, size_t *token_offset, size_t num_tokens) {
  size_t start = buf - very_start_buf;
  size_t end = request_str_len;

  if (end - start > 0) {
    http_parse_token_t *token_to_fill = bounds_check_lookup_token(tokens, *token_offset, num_tokens);
    if (token_to_fill) {
      fill_token(start, end, HTTP_BODY, token_to_fill);
    }
    ++(*token_offset);
  }
  return very_start_buf + end;
}

int parse_request_with_sizes(char *request_str, size_t request_str_len, http_parse_token_t *tokens, size_t *num_tokens) {
  char *cur = request_str;

  char *separators[] = {" ", " ", "\r\n"};
  http_parse_token_type_t token_types[] = {HTTP_METHOD, HTTP_PATH, HTTP_PROTOCOL};

  int num_separators = sizeof(separators) / sizeof(separators[0]);

  size_t token_offset = 0;

  for (int i = 0; i < num_separators; i++) {
    http_parse_token_t *token_to_fill = NULL;
    if (tokens) {
      token_to_fill = bounds_check_lookup_token(tokens, i, *num_tokens);
    }
    cur = chomp_and_get_token(cur, separators[i], cur - request_str, token_types[i], token_to_fill);
    if (!cur) return -1;
    ++token_offset;
  }

  cur = read_headers(request_str, cur, tokens, &token_offset, *num_tokens);

  cur = get_body_token_if_exists(request_str, cur, request_str_len, tokens, &token_offset, *num_tokens);

  if (!tokens) {
    (*num_tokens) = token_offset;
  }

  return 0;
}

int parse_request(char *request_str, size_t request_str_len, http_parse_token_t *tokens, size_t num_tokens) {
  return parse_request_with_sizes(request_str, request_str_len, tokens, &num_tokens);
}
