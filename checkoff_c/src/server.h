#ifndef CHECKOFF_SERVER_H
#define CHECKOFF_SERVER_H

#include <stddef.h>
#include <microhttpd.h>

typedef struct MHD_Daemon Server_Daemon;

typedef enum {
  METHOD_GET,
  METHOD_POST
} HttpMethod;

typedef int (*HttpHandler)(char *post_data, char *response, size_t response_size);

int server_register_endpoint(HttpMethod method, const char *url, HttpHandler handler_function, int response_requires_free);

Server_Daemon *server_start_daemon(unsigned int port);
void server_stop_daemon(Server_Daemon *d);

enum MHD_Result dispatch_request(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** con_cls);

#endif
