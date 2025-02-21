#ifndef MYHTTP_H
#define MYHTTP_H

#include "mystdlib.h"

// "PROPPATCH" is currently longest known (10 chars), but use more just in case
#define MAX_HTTP_METHOD_LENGTH 16

// "HTTP/1.1" is max standard value (8 chars), but use more to support nonstandard requests
#define MAX_HTTP_PROTOCOL_LENGTH 32

// No set length, just set a reasonable amount
#define MAX_HTTP_PATH_LENGTH 512

/* //unused
typedef enum {
  HTTP_METHOD_UNSUPPORTED = 0,
  HTTP_METHOD_GET = 1 << 0,
  HTTP_METHOD_POST =  1 << 1
} http_method_t;

typedef struct {
  http_method_t method;
} http_request_t;

char *http_method_to_string(char *dest, http_method_t method, size_t n);
*/
int parse_request_fill_sizes(char *request_str,
                             char *method, size_t *method_size,
                             char *path, size_t *path_size,
                             char *protocol, size_t *protocol_size,
                             char **headers, size_t *num_headers,
                             size_t *header_sizes);

int parse_request(char *request_str,
                  char *method, size_t method_size,
                  char *path, size_t path_size,
                  char *protocol, size_t protocol_size);

#endif
