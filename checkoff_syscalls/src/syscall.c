#include "syscall.h"
#include "nolibc_includer.h"
#include <asm/unistd.h>

ssize_t write(int fd, const void *buf, size_t count) {
    return my_syscall3(__NR_write, fd, (long)buf, (long)count);
}

int socket(int domain, int type, int protocol) {
    return my_syscall3(__NR_socket, domain, type, protocol);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    return my_syscall5(__NR_setsockopt, sockfd, level, optname, (long)optval, optlen);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return my_syscall3(__NR_bind, sockfd, (long)addr, (long)addrlen);
}

int listen(int sockfd, int backlog) {
    return my_syscall2(__NR_listen, sockfd, backlog);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return my_syscall3(__NR_accept, sockfd, (long)addr, (long)addrlen);
}

ssize_t read(int fd, void *buf, size_t count) {
    return my_syscall3(__NR_read, fd, (long)buf, count);
}

int close(int fd) {
      return my_syscall1(__NR_close, fd);
}

__attribute__((noreturn)) void _exit(int status) {
      my_syscall1(__NR_exit, status);
      __builtin_unreachable();
}
