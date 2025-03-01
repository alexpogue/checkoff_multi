#ifndef MYHTTPREQUEST
#define MYHTTPREQUEST

#include "myhttp.h"

#define ENUM_COUNT(name) (name##_COUNT)

typedef enum {
  HEADER_ACCEPT,
  HEADER_AUTHORIZATION,
  HEADER_CONTENT_TYPE,
  HEADER_USER_AGENT,
  HEADER_COUNT, // so that we can retrieve number of "real" elements in this enum
  HEADER_EMPTY,
  HEADER_UNCATEGORIZED
} request_header_type_t;

typedef struct {
  http_parse_token_t key;
  http_parse_token_t value;
  request_header_type_t type;
} request_header_t;

typedef struct {
  http_parse_token_t protocol;
  http_parse_token_t method;
  http_parse_token_t path;
  http_parse_token_t body;
  request_header_t *headers;
  size_t num_uncategorized_headers;
} request_t;

int create_request_from_tokens(char *request_str, http_parse_token_t *tokens, size_t num_tokens, size_t headers_size, request_t *output);
char *get_key_string_from_header_type(request_header_type_t type, char *output, size_t output_length);

#endif
