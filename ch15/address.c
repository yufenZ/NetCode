//
//  main.c
//  address
//
//  Created by yufenZ on 2020/5/13.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#define SERV_PORT 43211
#define LISTENQ 1024
#define MAXLINE 4096

static int count;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

void error(int status, int err, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if(err)
        fprintf(stderr, ": %s (%d)\n", strerror(err), err);
    if(status)
        exit(status);
}

int main(int argc, const char * argv[]) {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int rt1 = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(rt1 < 0) {
        error(1, errno, "bind failed");
    }
    
    int rt2 = listen(listen_fd, LISTENQ);
    if(rt2 < 0) {
        error(1, errno, "listen failed");
    }
    
    signal(SIGPIPE, SIG_IGN);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if((connfd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        error(1, errno, "bind failed");
    }
    
    char message[MAXLINE];
    count = 0;
    
    for(;;) {
        ssize_t n = read(connfd, message, MAXLINE);
        if(n < 0) {
            error(1, errno, "error read");
        } else if (n == 0) {
            error(1, 0, "client close\n");
        }
        message[n] = 0;
        printf("received %zd bytes: %s\n", n, message);
        count ++;
    }
    return 0;
}
