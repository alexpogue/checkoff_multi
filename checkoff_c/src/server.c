#include "server.h"
#include <microhttpd.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  HttpMethod method;
  const char *url;
  HttpHandler handler_function;
  int response_requires_free;
} Endpoint;

struct connection_info {
  char *post_data;
  size_t post_data_size;
};

#define RESPONSE_SIZE 1000
#define MAX_ENDPOINTS 1000
static Endpoint endpoints[MAX_ENDPOINTS];
static size_t endpoint_count = 0;

Server_Daemon *server_start_daemon(unsigned int port) {
  return MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION,
		       port,
		       NULL,
		       NULL,
		       &dispatch_request,
		       NULL,
		       MHD_OPTION_END);
}

void server_stop_daemon(Server_Daemon *d) {
  MHD_stop_daemon(d);
}

int server_register_endpoint(HttpMethod method, const char *url, HttpHandler handler_function, int response_requires_free) {
    if (endpoint_count >= MAX_ENDPOINTS) {
        return -1; // Error: too many endpoints
    }
    endpoints[endpoint_count].method = method;
    endpoints[endpoint_count].url = url;
    endpoints[endpoint_count].handler_function = handler_function;
    endpoints[endpoint_count].response_requires_free = response_requires_free;
    endpoint_count++;
    return 0;
}

static int match_endpoint(const char *url, const char *method, Endpoint *matched) {
  for (size_t i = 0; i < endpoint_count; i++) {
    if (strcmp(url, endpoints[i].url) == 0 &&
        ((strcmp(method, "POST") == 0 && endpoints[i].method == METHOD_POST) ||
         (strcmp(method, "GET") == 0 && endpoints[i].method == METHOD_GET))) {
      *matched = endpoints[i];
      return 1;
    }
  }
  return 0;
}

enum MHD_Result dispatch_request(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** con_cls) {

  Endpoint endpoint;
  int endpoint_found = match_endpoint(url, method, &endpoint);
  if (!endpoint_found) {
    printf("Endpoint not found for url %s %s\n", method, url);
    return MHD_NO;
  }

  struct connection_info *con_info;

  int isPostRequest = (0 == strcmp(method, "POST"));
  int isGetRequest = (0 == strcmp(method, "GET"));

  if (!isPostRequest && !isGetRequest)
    return MHD_NO; /* unexpected method */

  if (*con_cls == NULL) {
    con_info = malloc(sizeof(struct connection_info));
    if (isPostRequest) {
      con_info->post_data = NULL;
      con_info->post_data_size = 0;
    }
    *con_cls = con_info;
    return MHD_YES;
  }

  con_info = *con_cls;

  if (isGetRequest && *upload_data_size != 0)
    return MHD_NO; /* reject upload data in a GET */

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
    char *response_str;
    char response_buffer[RESPONSE_SIZE];

    printf("Handling %s %s\n", method, url);
    response_str = endpoint.handler_function(con_info->post_data, response_buffer, RESPONSE_SIZE);

    if (!response_str) {
      response_str = response_buffer; // Use the buffer if no string is returned
    }

    struct MHD_Response *response;
    if (endpoint.response_requires_free) {
      response = MHD_create_response_from_buffer(strlen(response_str), response_str, MHD_RESPMEM_MUST_FREE);
    } else {
      response = MHD_create_response_from_buffer(strlen(response_str), response_str, MHD_RESPMEM_MUST_COPY);
    }

    int ret = MHD_queue_response(connection,
                   MHD_HTTP_OK,
                   response);
    MHD_destroy_response(response);
    if (con_info->post_data) {
      free(con_info->post_data);
      con_info->post_data = NULL;
    }
    if (con_info) {
      free(con_info);
      con_info = NULL;
    }
    *con_cls = NULL;

    return ret;
  }
}
