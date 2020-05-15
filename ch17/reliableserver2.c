//
//  main.c
//  reliableserver2
//
//  Created by yufenZ on 2020/5/14.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define SERV_PORT 43211
#define LISTENQ 1024

void error(int status, int err, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if(err) {
        fprintf(stderr, "%s, (%d)\n", strerror(err), err);
    }
    if(status)
        exit(status);
}

int tcp_server(int port){
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    
    int rt1 = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(rt1 < 0)
        error(1, errno, "bind failed");
    
    int rt2 = listen(listen_fd, LISTENQ);
    if(rt2 < 0)
        error(1, errno, "listen failed");
    
    signal(SIGPIPE, SIG_IGN);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if((connfd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0)
        error(1, errno, "bind failed");
    
    return connfd;
}

int main(int argc, char **argv) {
    int connfd;
    char buf[1024];
    int time = 0;
    
    connfd = tcp_server(SERV_PORT);
    
    while(1) {
        ssize_t n = read(connfd, buf, 1024);
        if(n < 0)
            error(1, errno, "error read");
        else if(n == 0)
            error(1, 0, "client close\n");
        
        time ++;
        fprintf(stdout, "1K read for %d \n", time);
        usleep(10000);
    }
    return 0;
}
