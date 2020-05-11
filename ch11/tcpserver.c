//
//  main.c
//  tcpserver
//
//  Created by yufenZ on 2020/5/11.
//  Copyright © 2020年 yufenZ. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> //exit()
#include <sys/socket.h> // socket()
#include <netinet/in.h> //sockaddr_in
#include <string.h> //bzero()
#include <stdarg.h> //va_start(), va_end()
#include <errno.h> //errno
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
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);
    
    int rt1 = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if(rt1 < 0) {
        error(1, errno, "bind failed");
    }
    int rt2 = listen(listenfd, LISTENQ);
    if(rt2 < 0){
        error(1, errno, "listen failed");
    }
    
    signal(SIGINT, sig_int);
    signal(SIGPIPE, SIG_DFL);
    
    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len =sizeof(client_addr);
    
    if((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0){
        error(1, errno, "bind failed");
    }
    
    char message[MAXLINE];
    count = 0;
    
    for(;;) {
        ssize_t n = read(connfd, message, MAXLINE);
        if(n < 0) {
            error(1, errno, "error read");
        } else if(n == 0){
            error(1, 0, "client closed \n");
        }
        message[n] = 0;
        printf("received %zu bytes: %s\n", n, message);
        count ++;
        
        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);
        
        sleep(5);
        
        ssize_t write_nc = send(connfd, send_line, strlen(send_line), 0);
        printf("send bytes: %zu \n", write_nc);
        if(write_nc < 0) {
            error(1, errno, "error write");
        }
    }
    return 0;
}
