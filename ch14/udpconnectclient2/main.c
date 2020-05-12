//
//  main.c
//  udpconnectclient2
//
//  Created by yufenZ on 2020/5/12.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NDG 2000
#define DGLEN 1400
#define MAXLINE 4096
#define SERV_PORT 43211

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

int main(int argc, char **argv) {
    if(argc != 2){
        error(1, 0, "usage: udpclient2 <IPaddress>");
    }
    
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    
    socklen_t server_len = sizeof(server_addr);
    
    if(connect(socket_fd, (struct sockaddr *) &server_addr, server_len)) {
        error(1, errno, "connect failed");
    }
    
    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    ssize_t n;
    
    while(fgets(send_line, MAXLINE, stdin) != NULL) {
        unsigned long i = strlen(send_line);
        if(send_line[i-1] == '\n') {
            send_line[i-1] = 0;
        }
        
        printf("now sending %s\n", send_line);
        size_t rt = send(socket_fd, send_line, strlen(send_line), 0);
        if(rt < 0){
            error(1, errno, "send failed");
        }
        printf("send bytes: %zu \n", rt);
        
        recv_line[0] = 0;
        n = recv(socket_fd, recv_line, MAXLINE, 0);
        if(n < 0) {
            error(1, errno, "recv failed");
        }
        recv_line[n] = 0;
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }
    exit(0);
    //return 0;
}
