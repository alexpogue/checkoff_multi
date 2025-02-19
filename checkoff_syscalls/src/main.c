#include <sys/socket.h>
#include <netinet/in.h>
#include "syscall.h"

#define PORT 8080

#define SYS_WRITE 1
#define SYS_EXIT 60
#define STDOUT_FILENO 1

static inline unsigned short my_htons(unsigned short x) {
      return (x << 8) | (x >> 8);
}

long my_strlen(const char *str) {
  if (str == NULL)
    return 0;
  long i = 0;
  for (; str[i] != '\0'; ++i) ;
  return i;
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

    log_message("Creating socket...\n");

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
      log_message("Socket creation failed.\n");
      _exit(1);
    }
    log_message("Setting socket options...\n");

    // Set socket options
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    log_message("Binding socket...\n");

    // Bind the socket to the network address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = my_htons(PORT);
    int bind_result = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (bind_result < 0) {
      log_message("Binding failed.\n");
      _exit(1);
    }

    log_message("Listening for connections...\n");

    // Listen for incoming connections
    listen(server_fd, 3);

    while (1) {
        // Accept an incoming connection
        socklen_t addrlen = sizeof(address);
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);

        // Read the request
        read(new_socket, buffer, sizeof(buffer) - 1);

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
