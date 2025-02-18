#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "json.h"
#include "server.h"

char* malloc_json_response(const char *status, const char *name) {
  char *response = NULL;
  if (name) {
    size_t message_str_len = strlen("{\"status\":\"\",\"name\":\"\"}") + strlen(status) + strlen(name);
    response = malloc(message_str_len + 1);
    if (response) {
      snprintf(response, message_str_len + 1, "{\"status\":\"%s\",\"name\":\"%s\"}", status, name);
    }
  } else {
    size_t message_str_len = strlen("{\"status\":\"\"}") + strlen(status);
    response = malloc(message_str_len + 1);
    if (response) {
      snprintf(response, message_str_len + 1, "{\"status\":\"%s\"}", status);
    }
  }
  return response;
}

char *handle_post_json(char *post_data, char *unused, size_t unused2) {
  int value_length = json_get_key_value("name", post_data, NULL, 0);

  char *response = NULL;
  if (value_length == -1) {
    printf("did not find key 'name'\n");
    response = malloc_json_response("success", NULL);
  } else if (value_length == -2) {
    printf("error parsing json\n");
    response = malloc_json_response("fail", NULL);
  } else if (value_length < 0) {
    printf("unknown error retrieving name from json");
    response = malloc_json_response("fail", NULL);
  } else {
    char *value_str = malloc(value_length + 1);
    if (value_str) {
      json_get_key_value("name", post_data, value_str, value_length + 1);
      response = malloc_json_response("success", value_str);
      free(value_str);
    }
  }
  return response;
}

char *handle_get_json(char *unused, char *response, size_t response_size) {
  strncpy(response, "{\"status\":\"success\",\"data\":[{\"id\": 5, \"name\": \"Do the dishes\", \"description\": \"Make them squeaky clean\"}]}", response_size);
  return NULL;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("%s PORT\n",
     argv[0]);
    return 1;
  }
  char *port_str = argv[1];

  Server_Daemon *d = server_start_daemon(atoi(port_str));
  if (d == NULL)
    return 1;

  server_register_endpoint(METHOD_POST, "/json", handle_post_json, 1);
  server_register_endpoint(METHOD_GET, "/todoitem", handle_get_json, 0);

  printf("Server listening on port %s\n", port_str);
  getc(stdin);
  server_stop_daemon(d);
  return 0;
}
