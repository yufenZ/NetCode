//
//  main.c
//  reliableclient1
//
//  Created by yufenZ on 2020/5/14.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 102400000
#define SERV_PORT 43211

void error(int status, int err, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if(err)
        fprintf(stderr, ": %s (%d)\n", strerror(err), err);
    if(status)
        exit(status);
}

int tcp_client(char *address, int port) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_addr.sin_addr);
    
    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if(connect_rt < 0) {
        error(1, errno, "connect failed");
    }
    return socket_fd;
}

int main(int argc, char **argv) {
    if(argc != 2) {
        error(1, 0, "usage: reliable_client1 <IPaddress>");
    }
    int socket_fd = tcp_client(argv[1], SERV_PORT);
    char buf[129];
    unsigned long len;
    ssize_t rc;
    
    while(fgets(buf, sizeof(buf), stdin) != NULL) {
        len = strlen(buf);
        rc = send(socket_fd, buf, len, 0);
        if(rc < 0) {
            error(1, errno, "write failed");
        }
        sleep(3);
        rc = read(socket_fd, buf, sizeof(buf));
        if(rc < 0) {
            error(1, errno, "read failed");
        } else if(rc == 0)
            error(1, 0, "peer connection closed\n");
        else
            fputs(buf, stdout);
    }
    exit(0);
    // return 0;
}
