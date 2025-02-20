#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include "json.h"
#include "server.h"

#define MAX_JSON_VALUE_SIZE 256
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

int print_license_notice() {
  puts("libmicrohttpd is LGPL licensed, source: https://www.gnu.org/software/libmicrohttpd/");
}

size_t concatenate_strings(char *dest, const char **strings, size_t num_strings, size_t dest_size) {
  if (!dest || !strings || num_strings == 0) {
    return 0;
  }
  size_t num_chars_copied = 0;
  for (int i = 0; i < num_strings; i++) {
    if (!strings[i]) continue;
    if (dest_size - num_chars_copied - 1 <= 0) break;
    strncpy(dest + num_chars_copied, strings[i], dest_size - num_chars_copied - 1);
    num_chars_copied += strlen(strings[i]);
  }
  dest[dest_size - 1] = '\0';
  return num_chars_copied;
}

int get_json_response(const char *status, const char *name, char *response, size_t size) {
  if (!status || !name || !response) {
    return -1;
  }
  int num_chars_attempted;
  if (name) {
    const char *strings[] = {"{\"status\":\"", status, "\",\"name\":\"", name, "\"}"};
    num_chars_attempted = concatenate_strings(response, strings, ARRAY_LENGTH(strings), size);
  } else {
    const char *strings[] = {"{\"status\":\"", status, "\"}"};
    num_chars_attempted = concatenate_strings(response, strings, ARRAY_LENGTH(strings), size);
  }
  if (size < num_chars_attempted) {
    return -2;
  }
  return num_chars_attempted;
}

char *handle_post_json(char *post_data, char *response, size_t response_size) {
  char value_str[MAX_JSON_VALUE_SIZE];
  int value_length = json_get_key_value("name", post_data, value_str, MAX_JSON_VALUE_SIZE);

  int json_response_ret;
  if (value_length == -1) {
    printf("did not find key 'name'\n");
    json_response_ret = get_json_response("success", NULL, response, response_size);
  } else if (value_length == -2) {
    printf("error parsing json\n");
    json_response_ret = get_json_response("fail", NULL, response, response_size);
  } else if (value_length < 0) {
    printf("unknown error retrieving name from json");
    json_response_ret = get_json_response("fail", NULL, response, response_size);
  } else {
    json_response_ret = get_json_response("success", value_str, response, response_size);
  }
  if (json_response_ret < 0) {
    json_response_ret = get_json_response("json_write_fail", NULL, response, response_size);
    if (json_response_ret < 0) {
      puts("Fatal error, can't fit error response in api output or error writing json. Abort.");
      exit(1);
    }
  }
  printf("response = %s\n", response);
  return NULL; // unused
}

char *handle_get_json(char *unused, char *response, size_t response_size) {
  strncpy(response, "{\"status\":\"success\",\"data\":[{\"id\": 5, \"name\": \"Do the dishes\", \"description\": \"Make them squeaky clean\"}]}", response_size);
  return NULL;
}

int read_int(const char *input, int *result) {
  char *endptr;
  long value;

  errno = 0;

  value = strtol(input, &endptr, 10);

  if (endptr == input) {
    return 1; // No digits were found
  } else if (*endptr != '\0') {
    return 2; // Non-numeric characters found in input
  } else if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN)) || (value > INT_MAX || value < INT_MIN)) {
    return 3; // The input value is out of the range of an integer
  }

  *result = (int)value;
  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s [ PORT | --license ]\n",
     argv[0]);
    return 1;
  }
  if (argc > 1 && strcmp(argv[1], "--license") == 0) {
    print_license_notice();
    return 0;
  }

  char *port_str = argv[1];

  int port_int;
  int ret_val = read_int(port_str, &port_int);
  if (ret_val != 0) {
    fprintf(stderr, "Error converting port \"%s\" to integer\n", port_str);
    printf("usage: %s [ PORT | --license ]\n", argv[0]);
    return 1;
  }

  Server_Daemon *d = server_start_daemon(port_int);
  if (d == NULL)
    return 1;

  server_register_endpoint(METHOD_POST, "/json", handle_post_json, 0);
  server_register_endpoint(METHOD_GET, "/todoitem", handle_get_json, 0);

  printf("Server listening on port %s\n", port_str);
  getc(stdin);
  server_stop_daemon(d);
  return 0;
}
