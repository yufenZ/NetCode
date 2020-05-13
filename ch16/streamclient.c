//
//  main.c
//  streamclient
//
//  Created by yufenZ on 2020/5/13.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

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
    if(argc != 2) {
        error(1, 0, "usage: tcpclient <IPaddress>");
    }
    
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    
    socklen_t server_len = sizeof(server_addr);
    int connet_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if(connet_rt < 0) {
        error(1, errno, "connect failed");
    }
    
    struct {
        u_int32_t message_length;
        u_int32_t message_type;
        char data[128];
    } message;
    
    unsigned long n;
    
    while(fgets(message.data, sizeof(message.data), stdin) != NULL) {
        n = strlen(message.data);
        message.message_length = htonl(n);
        message.message_type = 1;
        if(send(socket_fd, (char *) &message, sizeof(message.message_length) + sizeof(message.message_type) + n, 0) < 0) {
            error(1, errno, "send failure");
        }
    }
    exit(0);
    //return 0;
}
