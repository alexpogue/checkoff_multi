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

__attribute__((noreturn)) void _exit(int status) {
      my_syscall_1(SYS_EXIT, status);
      __builtin_unreachable();
}

static inline long my_syscall_1(long number, long arg1) {
    register long rax asm("rax") = number;
    register long rdi asm("rdi") = arg1;
    asm volatile (
        "syscall"
        : "+r"(rax)
        : "r"(rdi)
        : "rcx", "r11", "memory"
    );
    return rax;
}

static inline long my_syscall_2(long number, long arg1, long arg2) {
    register long rax asm("rax") = number;
    register long rdi asm("rdi") = arg1;
    register long rsi asm("rsi") = arg2;
    asm volatile (
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi)
        : "rcx", "r11", "memory"
    );
    return rax;
}

static inline long my_syscall_3(long number, long arg1, long arg2, long arg3) {
    register long rax asm("rax") = number;
    register long rdi asm("rdi") = arg1;
    register long rsi asm("rsi") = arg2;
    register long rdx asm("rdx") = arg3;
    asm volatile (
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx)
        : "rcx", "r11", "memory"
    );
    return rax;
}

static inline long my_syscall_5(long number, long arg1, long arg2, long arg3, long arg4, long arg5) {
    register long rax asm("rax") = number;
    register long rdi asm("rdi") = arg1;
    register long rsi asm("rsi") = arg2;
    register long rdx asm("rdx") = arg3;
    register long r10 asm("r10") = arg4;
    register long r8  asm("r8")  = arg5;
    asm volatile (
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8)
        : "rcx", "r11", "memory"
    );
    return rax;
}

/*
static inline long my_syscall_6(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    register long rax asm("rax") = number;
    register long rdi asm("rdi") = arg1;
    register long rsi asm("rsi") = arg2;
    register long rdx asm("rdx") = arg3;
    register long r10 asm("r10") = arg4;
    register long r8  asm("r8")  = arg5;
    register long r9  asm("r9")  = arg6;
    asm volatile (
        "syscall"
        : "+r"(rax)
        : "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory"
    );
    return rax;
}
*/
