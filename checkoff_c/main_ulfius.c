/**
 * test.c
 * Small Hello World! example
 * to compile with gcc, run the following command
 * gcc -o test test.c -lulfius
 */
#include <stdio.h>
#include <ulfius.h>

#define PORT 8080

/**
 * Callback function for the web application on /helloworld url call
 */
int callback_hello_world (const struct _u_request * request, struct _u_response * response, void * user_data) {
  ulfius_set_string_body_response(response, 200, "Hello World!");
  return U_CALLBACK_CONTINUE;
}

int post_json_string_and_return_json(const struct _u_request *request, struct _u_response *response, void *user_data) {
  json_t *json_request_body = ulfius_get_json_body_request(request, NULL);
  json_t *json_response_body = NULL;

  const char *name = json_string_value(json_object_get(json_request_body, "name"));
  json_response_body = json_object();
  json_t *json_str = json_string(name);

  json_object_set_new(json_response_body, "status", json_string("success"));
  json_object_set_new(json_response_body, "name", json_string(name));
  ulfius_set_json_body_response(response, 200, json_response_body);
  json_decref(json_request_body);
  json_decref(json_response_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * main function
 */
int main(void) {
  struct _u_instance instance;

  // Initialize instance with the port number
  if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK) {
    fprintf(stderr, "Error ulfius_init_instance, abort\n");
    return(1);
  }

  // Endpoint list declaration
  ulfius_add_endpoint_by_val(&instance, "GET", "/helloworld", NULL, 0, &callback_hello_world, NULL);
  ulfius_add_endpoint_by_val(&instance, "POST", "/json", NULL, 0, &post_json_string_and_return_json, NULL);

  // Start the framework
  if (ulfius_start_framework(&instance) == U_OK) {
    printf("Start framework on port %d\n", instance.port);

    // Wait for the user to press <enter> on the console to quit the application
    getchar();
  } else {
    fprintf(stderr, "Error starting framework\n");
  }
  printf("End framework\n");

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);

  return 0;
}
