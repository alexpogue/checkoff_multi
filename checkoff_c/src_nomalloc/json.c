#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "jsmn.h"

#define MAX_JSON_TOKEN_SIZE 1024
#define MAX_SIZE_TOKEN_CONTAINER 1024

int get_token_string(const char *json, jsmntok_t *tok, char *output, size_t size) {
  if (!json || !tok) return -1;

  if (tok->end < 0 || tok->start < 0) {
    printf("error: token bounds are negative, usually means error parsing. tok->start = %d, tok->end = %d\n", tok->start, tok->end);
    return -2;
  }

  if (tok->end < tok->start) {
    printf("error: token ends before it starts\n");
    return -3;
  }
  size_t length = tok->end - tok->start;
  //printf("length: %d\n", length);

  if (output != NULL) {
    if (length >= size) {
      length = size - 1;
    }

    strncpy(output, json + tok->start, length);
    output[length] = '\0';
  }
  return length;
}

char *get_token_string_simple(const char *json, jsmntok_t *tok, char *output, size_t size) {
  if(get_token_string(json, tok, output, size) < 0)
    return NULL;
  return output;
}

/*
void print_token(const char *json, jsmntok_t *tok) {
  char token_str[MAX_JSON_TOKEN_SIZE];
  char *token_ptr = get_token_string_simple(json, tok, token_str, MAX_JSON_TOKEN_SIZE);
  if (!token_ptr) {
    printf("Error parsing, can't print\n");
  } else {
    printf("%s\n", token_ptr);
  }
}
*/

int get_num_tokens_in_container(const char *json, size_t token_index, jsmntok_t *tokens, size_t tokens_size) {
  if (token_index >= tokens_size) {
    printf("error: passed an out-of-bounds value to get_num_tokens_in_container. Passed index: %zu, size:%zu\n", token_index, tokens_size);
    return -1;
  }
  jsmntok_t *container_token = &tokens[token_index];
  if (container_token->type != JSMN_ARRAY && container_token->type != JSMN_OBJECT) {
    printf("error: sent a non-container type into get_num_tokens_in_container.. Type = %d\n", container_token->type);
    return -2;
  }
  //size_t cur_element_index = token_index + 1;
  size_t i = token_index + 1;
  for (; i < tokens_size && tokens[i].start < container_token->end; i++) { };
  return i - token_index - 1;
}

int get_token_length(jsmntok_t *tok) {
  return tok->end - tok->start;
}

int token_matches_target_key(char *json, jsmntok_t *key_tok, char *key_target) {
  char key_str[MAX_JSON_TOKEN_SIZE];
  char *key_str_ptr = get_token_string_simple(json, key_tok, key_str, MAX_JSON_TOKEN_SIZE);
  if (!key_str_ptr) {
    printf("error getting key token string\n");
    return 0;
  }
  return (0 == strcmp(key_str, key_target));
}

int json_get_key_value(char *key, char *json, char *output, size_t output_size) {
  jsmn_parser parser;
  jsmn_init(&parser);
  jsmntok_t tokens[MAX_SIZE_TOKEN_CONTAINER];
  int num_tokens = jsmn_parse(&parser, json, strlen(json), tokens, MAX_SIZE_TOKEN_CONTAINER);
  if (num_tokens < 0) {
    printf("error parsing json\n");
    return -2;
  }
  int i = 1;
  while (i < num_tokens) {
    jsmntok_t *key_token = &tokens[i];
    if(token_matches_target_key(json, key_token, key)) {
      if (output != NULL) {
        // `output` is passsed as an output parameter
        return get_token_string(json, &tokens[i + 1], output, output_size);
      } else {
        return get_token_string(json, &tokens[i + 1], NULL, 0);
      }
    }
    //printf("key: ");
    //print_token(json, &tokens[i]);
    i++;
    jsmntok_t *value_ptr = &tokens[i];
    //printf("Value: ");
    //print_token(json, value_ptr);
    if (value_ptr->type == JSMN_ARRAY || value_ptr->type == JSMN_OBJECT) {
      int num_tokens_in_container = get_num_tokens_in_container(json, i, tokens, num_tokens);
      printf("num_tokens_in_container = %d\n", num_tokens_in_container);
      i += (1 + num_tokens_in_container);
    } else {
      i += 1;
    }
  }
  return -1; // did not find value
}
