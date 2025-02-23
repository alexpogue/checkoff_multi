#include "syscall.h"

#define SYS_WRITE 1
#define SYS_SOCKET 41
#define SYS_SETSOCKOPT 54
#define SYS_BIND 49
#define SYS_LISTEN 50
#define SYS_ACCEPT 43
#define SYS_READ 0
#define SYS_CLOSE 3
#define SYS_EXIT 60


//static long my_syscall_0(long number);
static long my_syscall_1(long number, long arg1);
static long my_syscall_2(long number, long arg1, long arg2);
static long my_syscall_3(long number, long arg1, long arg2, long arg3);
//static long my_syscall_4(long number, long arg1, long arg2, long arg3, long arg4);
static long my_syscall_5(long number, long arg1, long arg2, long arg3, long arg4, long arg5);
//static long my_syscall_6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6);

ssize_t write(int fd, const void *buf, size_t count) {
    return my_syscall_3(SYS_WRITE, fd, (long)buf, (long)count);
}

int socket(int domain, int type, int protocol) {
    return my_syscall_3(SYS_SOCKET, domain, type, protocol);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    return my_syscall_5(SYS_SETSOCKOPT, sockfd, level, optname, (long)optval, optlen);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return my_syscall_3(SYS_BIND, sockfd, (long)addr, (long)addrlen);
}

int listen(int sockfd, int backlog) {
    return my_syscall_2(SYS_LISTEN, sockfd, backlog);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return my_syscall_3(SYS_ACCEPT, sockfd, (long)addr, (long)addrlen);
}

ssize_t read(int fd, void *buf, size_t count) {
    return my_syscall_3(SYS_READ, fd, (long)buf, count);
}

int close(int fd) {
      return my_syscall_1(SYS_CLOSE, fd);
}

void _exit(int status) {
      my_syscall_1(SYS_EXIT, status);
      while (1) {
        // function should not get here, but just in case, to solve `warning: ‘noreturn’ function does return` gcc warning
      }
}

static inline long my_syscall_1(long number, long arg1) {
    long ret;
    asm volatile (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(number), "r"(arg1)
        : "memory", "rax", "rdi"
    );
    return ret;
}

static inline long my_syscall_2(long number, long arg1, long arg2) {
    long ret;
    asm volatile (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "movq %3, %%rsi\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(number), "r"(arg1), "r"(arg2)
        : "memory", "rax", "rdi", "rsi"
    );
    return ret;
}

static inline long my_syscall_3(long number, long arg1, long arg2, long arg3) {
    long ret;
    asm volatile (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "movq %3, %%rsi\n"
        "movq %4, %%rdx\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3)
        : "memory", "rax", "rdi", "rsi", "rdx"
    );
    return ret;
}

static inline long my_syscall_5(long number, long arg1, long arg2, long arg3, long arg4, long arg5) {
    long ret;
    asm volatile (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "movq %3, %%rsi\n"
        "movq %4, %%rdx\n"
        "movq %5, %%r10\n"
        "movq %6, %%r8\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5)
        : "memory", "rax", "rdi", "rsi", "rdx", "r10", "r8"
    );
    return ret;
}

/*
static inline long my_syscall_6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    long ret;
    asm volatile (
        "movq %1, %%rax\n"
        "movq %2, %%rdi\n"
        "movq %3, %%rsi\n"
        "movq %4, %%rdx\n"
        "movq %5, %%r10\n"
        "movq %6, %%r8\n"
        "movq %7, %%r9\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5), "r"(arg6)
        : "memory", "rax", "rdi", "rsi", "rdx", "r10", "r8
    );
    return ret;
}
*/
