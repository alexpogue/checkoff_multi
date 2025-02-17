#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "json.h"
#include <microhttpd.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>"\
             "</head><body>libmicrohttpd demo</body></html>"
#define JSON_TOKEN_MAX_LENGTH 1000

struct connection_info {
  char *post_data;
  size_t post_data_size;
};

void free_memory_callback(void *data) {
  free(data);
}

static enum MHD_Result handle_post(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** con_cls) {
  struct connection_info *con_info;

  if (0 != strcmp(method, "POST"))
    return MHD_NO; /* unexpected method */

  if (*con_cls == NULL) {
    con_info = malloc(sizeof(struct connection_info));
    con_info->post_data = NULL;
    con_info->post_data_size = 0;
    *con_cls = con_info;
    return MHD_YES;
  }

  con_info = *con_cls;

  if (*upload_data_size) {
    con_info->post_data = realloc(con_info->post_data,
                                  con_info->post_data_size + *upload_data_size + 1);
    memcpy(con_info->post_data + con_info->post_data_size,
           upload_data, *upload_data_size);
    con_info->post_data_size += *upload_data_size;
    con_info->post_data[con_info->post_data_size] = '\0';
    *upload_data_size = 0;
    return MHD_YES;
  }
  else {
    printf("received POST data: %s\n", con_info->post_data);

    struct MHD_Response *response;

    int value_length = json_get_key_value("name", con_info->post_data, NULL, 0);
    printf("value_length = %d\n", value_length);

    char *response_str = NULL;
    if (value_length == -1) {
      printf("did not find key 'name'\n");
      char *tmp_response = "{\"status\":\"success\"}";
      int response_len = strlen(tmp_response);
      response_str = malloc(response_len + 1);
      snprintf(response_str, response_len + 1, "%s", response_str);
    } else if (value_length == -2) {
      printf("error parsing json\n");
      char *tmp_response = "{\"status\":\"fail\"}";
      int response_len = strlen(tmp_response);
      response_str = malloc(response_len + 1);
      snprintf(response_str, response_len + 1, "%s", response_str);
    } else {
      char *value_str = malloc(value_length + 1);
      json_get_key_value("name", con_info->post_data, value_str, value_length + 1);

      char *message_before_insert = "{\"status\":\"success\",\"name\":\"";
      char *message_after_insert = "\"}";
      int full_message_len = strlen(message_before_insert) + value_length + strlen(message_after_insert);
      response_str = malloc(full_message_len + 1);
      int result = snprintf(response_str, full_message_len + 1, "%s%s%s", message_before_insert, value_str, message_after_insert);
      if (result >= full_message_len + 1) {
        printf("error: output json was truncated, malloc'd %d bytes, but required %d bytes\n", full_message_len + 1, result + 1);
      }

      free(value_str);
      value_str = NULL;
    }
    printf("response_str = %s\n", response_str);
    response = MHD_create_response_from_buffer_with_free_callback(strlen(response_str), response_str, &free_memory_callback);

    //char *message = "POST received, had errors";
    //response = MHD_create_response_from_buffer(strlen(message), message, MHD_RESPMEM_PERSISTENT);

    int ret = MHD_queue_response(connection,
                   MHD_HTTP_OK,
                   response);
    MHD_destroy_response(response);
    free(con_info->post_data);
    free(con_info);
    *con_cls = NULL;

    return ret;
  }
}

int main(int argc,
	 char ** argv) {
  struct MHD_Daemon * d;
  if (argc != 2) {
    printf("%s PORT\n",
	   argv[0]);
    return 1;
  }
  char *port_str = argv[1];
  d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		       atoi(port_str),
		       NULL,
		       NULL,
		       &handle_post,
		       PAGE,
		       MHD_OPTION_END);
  if (d == NULL)
    return 1;
  printf("Server listening on port %s\n", port_str);
  (void) getc (stdin);
  MHD_stop_daemon(d);
  return 0;
}
