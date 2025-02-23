#include <sys/socket.h>
#include <netinet/in.h>
#include "syscall.h"
#include "mystdlib.h"
#include "myhttp.h"

#define PORT 8080

#define MAX_NUM_TOKENS 128
#define MAX_TOKEN_TYPE_LENGTH 16

int token_type_to_string(http_parse_token_type_t token_type, char *out_str, size_t out_str_len) {
  if (token_type == HTTP_METHOD)
    my_strncpy(out_str, "method", out_str_len);
  else if (token_type == HTTP_PATH)
    my_strncpy(out_str, "path", out_str_len);
  else if (token_type == HTTP_PROTOCOL)
    my_strncpy(out_str, "protocol", out_str_len);
  else if (token_type == HTTP_HEADER_KEY)
    my_strncpy(out_str, "header key", out_str_len);
  else if (token_type == HTTP_HEADER_VALUE)
    my_strncpy(out_str, "header value", out_str_len);
  else if (token_type == HTTP_BODY)
    my_strncpy(out_str, "body", out_str_len);
  else
    my_strncpy(out_str, "unknown", out_str_len);
  return 0;
}

void put_substring(char *str, size_t start, size_t end) {
  for (int i = start; i < end; i++) {
    my_putchar(str[i]);
  }
  my_putchar('\n');
}

void _start() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024];
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";
/*
    // START MYSTDLIB TESTS

    char *hello_str = "hello";
    size_t hello_len = my_strlen(hello_str);

    int cmp_val = my_strncmp("", "", 5);
    if (cmp_val == 0) {
      my_puts("equal");
    } else if (cmp_val > 0) {
      my_puts("hello > hel");
    } else {
      my_puts("hello < hel");
    }

    my_puts("strstr");
    char *haystack = "hello";
    char *found = my_strstr(haystack, "helloo");
    if (!found)
      my_puts("not found");
    else
      my_puts(found);

    char *str = "hello";
    char *pos = my_memchr(str, 'l', my_strlen(str));
    my_puts("index of: ");
    if (pos != NULL)
      print_int(pos - str);
    else
      my_puts("NULL");

    char dest[11];
    my_strncpy(dest, "hello", hello_len + 1);
    my_puts(dest);
    char *src = "world";
    my_strncat(dest, src, sizeof(dest) - my_strlen(dest) - 1);
    my_puts(dest);

    int i = 2147483647;
    print_int(i);
    print_int(sizeof(int));

    // END MYSTDLIB TESTS
*/

    my_puts("Creating socket...");

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
      my_puts("Socket creation failed.");
      _exit(1);
    }
    my_puts("Setting socket options...");

    // Set socket options
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    my_puts("Binding socket...");

    // Bind the socket to the network address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = my_htons(PORT);
    int bind_result = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (bind_result < 0) {
      my_puts("Binding failed.");
      _exit(1);
    }

    my_puts("Listening for connections...");

    // Listen for incoming connections
    listen(server_fd, 3);

    while (1) {
        // Accept an incoming connection
        socklen_t addrlen = sizeof(address);
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        my_puts("got request");

        // Read the request
        ssize_t bytes_read = read(new_socket, buffer, sizeof(buffer) - 1);
        if (bytes_read < sizeof(buffer) - 1)
          buffer[bytes_read] = '\0';
        else
          my_puts("Error: buffer overloaded, need more space in buffer");

        my_puts(buffer);

        char method[MAX_HTTP_METHOD_LENGTH + 1];
        char path[MAX_HTTP_PATH_LENGTH + 1];
        char protocol[MAX_HTTP_PROTOCOL_LENGTH + 1];

        http_parse_token_t tokens[MAX_NUM_TOKENS];
        size_t num_tokens;

        int ret = parse_request_with_sizes(buffer, my_strlen(buffer), NULL, &num_tokens);
        if (ret != 0) {
          my_puts("error parsing");
        }

        ret = parse_request(buffer, my_strlen(buffer), tokens, MAX_NUM_TOKENS);
        if (ret != 0) {
          my_puts("error parsing");
        }
        for(int i = 0 ; i < num_tokens; i++ ) {
          char token_type_str[MAX_TOKEN_TYPE_LENGTH];
          int ret = token_type_to_string(tokens[i].type, token_type_str, MAX_TOKEN_TYPE_LENGTH);
          if (ret < 0) {
            my_puts("error converting token type to string");
          }
          my_puts(token_type_str);
          put_substring(buffer, tokens[i].start, tokens[i].end);
        }

        // Send the response
        write(new_socket, response, my_strlen(response));

        // Close the socket
        close(new_socket);
    }

    // Close the server socket
    close(server_fd);

    // Exit the program
    _exit(0);
}
