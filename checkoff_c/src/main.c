#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "json.h"
#include "server.h"

int handle_post_json(char *post_data, char *response, size_t response_size) {
  int value_length = json_get_key_value("name", post_data, NULL, 0);

  if (value_length == -1) {
    printf("did not find key 'name'\n");
    char *tmp_response = "{\"status\":\"success\"}";
    strncpy(response, tmp_response, response_size);
  } else if (value_length == -2) {
    printf("error parsing json\n");
    char *tmp_response = "{\"status\":\"fail\"}";
    strncpy(response, tmp_response, response_size);
  } else {
    char *value_str = malloc(value_length + 1);
    json_get_key_value("name", post_data, value_str, value_length + 1);

    char *message_before_insert = "{\"status\":\"success\",\"name\":\"";
    char *message_after_insert = "\"}";
    int result = snprintf(response, response_size, "%s%s%s", message_before_insert, value_str, message_after_insert);
    free(value_str);
    value_str = NULL;
  }
  return 1;
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

  server_register_endpoint(METHOD_POST, "/json", handle_post_json, 0);

  printf("Server listening on port %s\n", port_str);
  getc(stdin);
  server_stop_daemon(d);
  return 0;
}
