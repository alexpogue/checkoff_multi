#ifndef MYHTTP_H
#define MYHTTP_H

#include "mystdlib.h"

// "PROPPATCH" is currently longest known (10 chars), but use more just in case
#define MAX_HTTP_METHOD_LENGTH 16

// "HTTP/1.1" is max standard value (8 chars), but use more to support nonstandard requests
#define MAX_HTTP_PROTOCOL_LENGTH 32

// No set length, just set a reasonable amount
#define MAX_HTTP_PATH_LENGTH 512

typedef enum {
  HTTP_UNDEFINED = 0,
  HTTP_METHOD = 1 << 0,
  HTTP_PATH = 1 << 1,
  HTTP_PROTOCOL = 1 << 2,
  HTTP_HEADER_KEY = 1 << 3,
  HTTP_HEADER_VALUE = 1 << 4,
  HTTP_BODY = 1 << 5
} http_parse_token_type_t;

typedef struct {
  size_t start;
  size_t end;
  http_parse_token_type_t type;
} http_parse_token_t;

int parse_request_with_sizes(char *request_str, size_t request_str_len, http_parse_token_t *tokens, size_t *num_tokens);
int parse_request(char *request_str, size_t request_str_len, http_parse_token_t *tokens, size_t num_tokens);

#endif
