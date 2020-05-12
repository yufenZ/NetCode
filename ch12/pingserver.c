//
//  main.c
//  pingserver
//
//  Created by yufenZ on 2020/5/12.
//  Copyright © 2020年 yufenZ. All rights reserved.
//
#include "messageobjecte.h"
#include <stdio.h>
#include <stdlib.h> //exit()
#include <stdarg.h> //va_start(), va_end()
#include <string.h> //strerror()
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#define SERV_PORT 43211
#define LISTENQ 1024

static int count;

static void sig_int(int singo) {
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

int main(int argc, char **argv) {
    if(argc != 2) {
        error(1, 0, "usage: tcpserver <sleepingtime>");
    }
    int sleepingTime = atoi(argv[1]);
    
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
        error(1, errno, "bind failed");
    }
    
    signal(SIGINT, sig_int);
    signal(SIGPIPE, SIG_IGN);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    if((connfd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        error(1, errno, "bind failed");
    }
    
    messageObject message;
    count = 0;
    
    for(;;) {
        ssize_t n = read(connfd, (char *) &message, sizeof(messageObject));
        if(n < 0) {
            error(1, errno, "error read");
        } else if (n == 0) {
            error(1, 0, "client closed \n");
        }
        
        printf("received %zd bytes\n", n);
        count ++;
        
        switch (ntohl(message.type)) {
            case MSG_TYPE1:
                printf("process MSG_TYPE1 \n");
                break;
                
            case MSG_TYPE2:
                printf("process MSG_TYPE2 \n");
                break;
                
            case MSG_PING: {
                messageObject pong_message;
                pong_message.type = MSG_PONG;
                sleep(sleepingTime);
                ssize_t rc = send(connfd, (char *)&pong_message, sizeof(pong_message), 0);
                if(rc < 0)
                    error(1, errno, "send failed");
                break;
            }
                
            default:
                error(1, 0, "unknown message type (%d)\n", ntohl(message.type));
        }
    }
    
    return 0;
}
