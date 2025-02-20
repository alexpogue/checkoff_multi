#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "jsmn.h"

#define MAX_JSON_TOKEN_SIZE 1024
#define MAX_SIZE_TOKEN_CONTAINER 1024

int get_token_string(const char *json, jsmntok_t *tok, char *output, size_t size) {
  if (tok->end < 0 || tok->start < 0) {
    printf("error: token bounds are negative, usually means error parsing. tok->start = %d, tok->end = %d\n", tok->start, tok->end);
    return -1;
  }

  if (tok->end < tok->start) {
    printf("error: token ends before it starts\n");
    return -2;
  }
  int length = tok->end - tok->start;
  //printf("length: %d\n", length);

  if (length >= size) {
    length = size - 1;
  }

  if (output != NULL) {
    strncpy(output, json + tok->start, length);
    output[length] = '\0';
  }
  return length;
}
void print_token(const char *json, jsmntok_t *tok) {
  char token_str[MAX_JSON_TOKEN_SIZE];
  int chars_read = get_token_string(json, tok, token_str, MAX_JSON_TOKEN_SIZE);
  if (chars_read < 0) {
    printf("Error parsing, can't print\n");
  } else {
    printf("%s\n", token_str);
  }
}

int get_num_tokens_in_object(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size);


int get_num_tokens_in_array(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size) {
  if (token_index >= tokens_size) {
    printf("error: passed an out-of-bounds value to get_num_tokens_in_array. Passed index: %d, size:%zu\n", token_index, tokens_size);
    return -1;
  }
  jsmntok_t *array_token = &tokens[token_index];
  if (array_token->type != JSMN_ARRAY) {
    printf("error: sent a non-array type into get_num_tokens_in_array.. Type = %d\n", array_token->type);
    return -2;
  }
  int first_element_index = token_index + 1;

  int count = 0;
  for (int i = 0; i < array_token->size; i++) {
    int cur_element_index = first_element_index + i;
    if (cur_element_index >= tokens_size) {
      printf("error: array element is out of bounds of tokens. Unknown how this happened\n");
      return -3;
    }
    jsmntok_t *cur_element_token = &tokens[cur_element_index];
    if (cur_element_token->type == JSMN_OBJECT) {
      count += (1 + get_num_tokens_in_object(json, cur_element_index, tokens, tokens_size));
    } else if (cur_element_token->type == JSMN_ARRAY) {
      count += (1 + get_num_tokens_in_array(json, cur_element_index, tokens, tokens_size));
    } else {
      count += 1;
    }
  }
  return count;
      
}

int get_token_length(jsmntok_t *tok) {
  return tok->end - tok->start;
}

char *outer_get_token_string(char *json, jsmntok_t *tok, char *token_str, size_t token_size) {
  int num_chars_to_read = get_token_length(tok);
  if (num_chars_to_read >= MAX_JSON_TOKEN_SIZE) {
    puts("Error: token_str_len is greater than MAX_JSON_TOKEN_SIZE");
    num_chars_to_read = MAX_JSON_TOKEN_SIZE - 1;
  }
  int num_chars_actually_read = get_token_string(json, tok, token_str, num_chars_to_read + 1);
  if (num_chars_actually_read < 0) {
    printf("error getting token string\n");
    return NULL;
  }
  return token_str;
}

int token_matches_target_key(char *json, jsmntok_t *key_tok, char *key_target) {
  char key_str[MAX_JSON_TOKEN_SIZE];
  char *key_str_ptr = outer_get_token_string(json, key_tok, key_str, MAX_JSON_TOKEN_SIZE);
  if (!key_str_ptr) {
    printf("error getting key token string\n");
    return 0;
  }
  int ret_val = (0 == strcmp(key_str, key_target));
  return ret_val;
}

void copy_to_output(char *output, char *source, size_t size) {
  if (output == NULL || source == NULL) {
    return;
  }

  int num_chars_to_copy = strlen(source);
  if (num_chars_to_copy >= size) {
    num_chars_to_copy = size - 1;
  }
  strncpy(output, source, num_chars_to_copy);
  output[num_chars_to_copy] = '\0';
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
      char value_str[MAX_JSON_TOKEN_SIZE];
      char *value_str_ptr = outer_get_token_string(json, &tokens[i + 1], value_str, MAX_JSON_TOKEN_SIZE);
      if (!value_str_ptr) {
        printf("error getting value string\n");
      }
      if (output != NULL) {
        copy_to_output(output, value_str, output_size);
      }
      int value_str_len = strlen(value_str);
      return value_str_len;
    }
    printf("key: ");
    print_token(json, &tokens[i]);
    i++;
    jsmntok_t *value_ptr = &tokens[i];
    printf("Value: ");
    print_token(json, value_ptr);
    if (value_ptr->type == JSMN_ARRAY) {
      int num_tokens_in_array = get_num_tokens_in_array(json, i, tokens, num_tokens);
      printf("num_tokens_in_array = %d\n", num_tokens_in_array);
      i += (1 + num_tokens_in_array);
    }
    else if (value_ptr->type == JSMN_OBJECT) {
      int num_tokens_in_object = get_num_tokens_in_object(json, i, tokens, num_tokens);
      i += (1 + num_tokens_in_object);
    } else {
      i += 1;
    }
  }
  return -1; // did not find value
}

int get_num_tokens_in_object(const char *json, int token_index, jsmntok_t *tokens, size_t tokens_size) {
  if (token_index >= tokens_size) {
    printf("error: passed an out-of-bounds value to get_num_tokens_in_object. Passed index: %d, size:%zu\n", token_index, tokens_size);
    return -1;
  }
  jsmntok_t *cur_token = &tokens[token_index];
  if (cur_token->type != JSMN_OBJECT) {
    printf("error: sent a non-object type into num_tokens_in_object.. Type = %d\n", cur_token->type);
    return -2;
  }
  int first_key_string_index = token_index + 1;
  int count = 0;
  for (int i = 0; i < cur_token->size; i++) {
    int cur_key_string_index = first_key_string_index + i;
    if (cur_key_string_index >= tokens_size) {
      printf("error: key string index is out of bounds. Unknown how this could happen.\n");
      return -3;
    }
    if (tokens[cur_key_string_index].type != JSMN_STRING) {
      printf("error: object key must be string.. Type = %d\n", tokens[cur_key_string_index].type);
      return -4;
    }
    int value_index = cur_key_string_index + 1;
    if (value_index >= tokens_size) {
      printf("error: object value token index is out of bounds\n");
      return -5;
    }
    count += 1; // for key
    jsmntok_t *value = &tokens[value_index];
    if (value->type == JSMN_ARRAY) {
      // `(1 + ...)` because whole array is a token too
      count += (1 + get_num_tokens_in_array(json, value_index, tokens, tokens_size));
    } else if (value->type == JSMN_OBJECT) {
      // `(1 + ...)` because whole object is a token too
      count += (1 + get_num_tokens_in_object(json, value_index, tokens, tokens_size));
    } else {
      count += 1;
    }
  }
  return count;
}
