//
//  main.c
//  diyclient
//
//  Created by yufenZ on 2020/5/15.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>

#define MAXLINE 1024

void error(int status, int err, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if(err)
        fprintf(stderr, "%s (%d) \n", strerror(err), err);
    if(status)
        exit(status);
}

int tcp_client(char *address, int port) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_adrr;
    bzero(&server_adrr, sizeof(server_adrr));
    server_adrr.sin_family = AF_INET;
    server_adrr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_adrr.sin_addr);
    
    socklen_t server_len = sizeof(server_adrr);
    int connfd = connect(socket_fd, (struct sockaddr *) &server_adrr, server_len);
    if(connfd < 0) {
        error(1, errno, "connect failed");
    }
    
    return socket_fd;
}

int main(int argc, char **argv) {
    if(argc != 3)
        error(1, 0, "usage: tcp_client <IPaddress> <port>");
    
    int port = atoi(argv[2]);
    int socket_fd = tcp_client(argv[1], port);
    
    char recv_line[MAXLINE], send_line[MAXLINE];
    ssize_t n;
    
    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);
    
    for(;;) {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if(rc <= 0) {
            error(1, errno, "select failed");
        }
        
        if(FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if(n < 0)
                error(1, errno, "read error");
            else if(n == 0){
                printf("server closed \n");
                break;
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }
        
        if(FD_ISSET(STDIN_FILENO, &readmask)) {
            if(fgets(send_line, MAXLINE, stdin) != NULL) {
                unsigned long i = strlen(send_line);
                if(send_line[i-1] == '\n')
                    send_line[i-1] = 0;
                
                if(strncmp(send_line, "quit", strlen(send_line)) == 0) {
                    if(shutdown(socket_fd, 1))
                        error(1, errno, "shutdown failed");
                }
                
                size_t rt = write(socket_fd, send_line, strlen(send_line));
                if(rt < 0)
                    error(1, errno, "write failed");
            }
        }
    }
    exit(0);
//    return 0;
}
