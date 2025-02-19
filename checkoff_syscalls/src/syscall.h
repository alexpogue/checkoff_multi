#ifndef SYSCALL_H
#define SYSCALL_H

#include <unistd.h>
#include <sys/socket.h>

//typedef unsigned int socklen_t;

long write(int fd, const void *buf, size_t count);
int socket(int domain, int type, int protocol);
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
long read(int fd, void *buf, unsigned long count);
long write(int fd, const void *buf, unsigned long count);
int close(int fd);
void _exit(int status);

#endif
