#include <sys/socket.h>
#include <netinet/in.h>
#include "syscall.h"
#include "mystdlib.h"

#define PORT 8080

#define REQUEST_GET 1
#define REQUEST_POST 2

typedef struct {
  int request_type;
  size_t body_start;
  size_t body_end;
} request_t;


int parse_request(char *request_str, request_t *request_out) {
  // TODO: Continue
  if (!request_str) {
    return -1;
  }
  return 0;
}

static void log_message(const char *message) {
    long the_strlen = my_strlen(message);
    long ret = write(STDOUT_FILENO, message, the_strlen);
    return;
}

void _start() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024];
    const char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";

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

    char dest[11];
    my_strncpy(dest, "hello", hello_len + 1);
    my_puts(dest);
    char *src = "world";
    my_strncat(dest, src, sizeof(dest) - my_strlen(dest) - 1);
    my_puts(dest);

    int i = 2147483647;
    char istr[12];
    int_to_str(i, istr);
    my_puts(istr);
    int_to_str(sizeof(int), istr);
    my_puts(istr);

    // END MYSTDLIB TESTS

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
        read(new_socket, buffer, sizeof(buffer) - 1);
        my_puts(buffer);

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
