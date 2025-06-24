#include "my_http_request.h"
#include "mystdlib.h"

request_header_type_t get_header_type_from_key(char *key_pos, size_t key_length) {
  if (0 == my_strncmp(key_pos, "Accept", key_length)) {
    return HEADER_ACCEPT;
  } else if (0 == my_strncmp(key_pos, "Authorization", key_length)) {
    return HEADER_AUTHORIZATION;
  } else if (0 == my_strncmp(key_pos, "Content-Type", key_length)) {
    return HEADER_CONTENT_TYPE;
  } else if (0 == my_strncmp(key_pos, "User-Agent", key_length)) {
    return HEADER_USER_AGENT;
  } else {
    return HEADER_UNCATEGORIZED;
  }
}

char *get_key_string_from_header_type(request_header_type_t type, char *output, size_t output_length) {
  if (type == HEADER_ACCEPT) {
    return my_strncpy(output, "Accept", output_length);
  } else if (type == HEADER_AUTHORIZATION) {
    return my_strncpy(output, "Authorization", output_length);
  } else if (type == HEADER_CONTENT_TYPE) {
    return my_strncpy(output, "Content-Type", output_length);
  } else if (type == HEADER_USER_AGENT) {
    return my_strncpy(output, "User-Agent", output_length);
  } else {
    my_puts("Error: couldn't find string from header type enum");
    return my_strncpy(output, "Error, Uncategorized", output_length);
  }
}

int create_header_from_tokens(char *request_str, http_parse_token_t *key, http_parse_token_t *value, request_header_t *header_result) {
  if (!request_str || !key || !value || !header_result) return -1;
  if (key->type != HTTP_HEADER_KEY) return -2;
  if (value->type != HTTP_HEADER_VALUE) return -3;
  header_result->key = *key;
  header_result->value = *value;
  header_result->type = get_header_type_from_key(request_str + key->start, key->end - key->start);
  return 0;
}

int insert_header_into_array(request_header_t header, request_header_t *headers, size_t *next_index, size_t headers_size) {
  size_t desired_index;
  if (header.type == HEADER_UNCATEGORIZED) {
    desired_index = *next_index;
    (*next_index)++;
  } else {
    desired_index = header.type; // use the enum number to store into a reserved space - poor man's hash map
  }
  if (desired_index >= headers_size) {
    my_puts("Warning: headers_size wasn't big enough to hold headers necessary");
    return -1;
  }
  headers[desired_index] = header;
  return 0;
}

int create_request_from_tokens(char *request_str, http_parse_token_t *tokens, size_t num_tokens, size_t headers_size, request_t *request_result) {
  // need to save the earlier indices for known header types
  size_t first_uncategorized_header_index = ENUM_COUNT(HEADER);

  //initialize first indices to `HEADER_EMPTY` so we know which ones weren't filled
  for (size_t i = 0; i < first_uncategorized_header_index; i++) {
    request_header_t empty_header = {0};
    empty_header.type = HEADER_EMPTY;

    request_result->headers[i] = empty_header;
  }
  size_t next_header_index = first_uncategorized_header_index;
  http_parse_token_t *cur = tokens;
  while (num_tokens--) {
    if (cur->type == HTTP_METHOD) {
      request_result->method = *cur;
    } else if (cur->type == HTTP_PATH) {
      request_result->path = *cur;
    } else if (cur->type == HTTP_PROTOCOL) {
      request_result->protocol = *cur;
    } else if (cur->type == HTTP_BODY) {
      request_result->body = *cur;
    } else if (cur->type == HTTP_HEADER_KEY) {
      if (!(num_tokens--)) {
        return -1; // expected value for the key but reached end of token array
      }
      http_parse_token_t *key = cur;
      http_parse_token_t *value = ++cur;
      if (value->type != HTTP_HEADER_VALUE) {
        return -2; // expected value for the key, but found another token type
      }

      // TODO: for some reason adding "= {0}" to the end of this line causes segfault.. It shouldn't.. probably a memory bug somewhere
      request_header_t header;
      create_header_from_tokens(request_str, key, value, &header);
      insert_header_into_array(header, request_result->headers, &next_header_index, headers_size);
    } else {
      my_puts("Error: Unhandled token type in create_request_from_tokens");
    }
    ++cur;
  }
  request_result->num_uncategorized_headers = next_header_index - first_uncategorized_header_index;
  return 0;
}
