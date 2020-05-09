#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <zconf.h>
#include <errno.h>

# define SERVER_PORT 12345

ssize_t readn(int fd, void *vptr, size_t size) {
    size_t nleft;
    ssize_t nread;
    char *ptr;
   
    ptr = vptr;
    nleft = size;
    while(nleft > 0) {
	if((nread = read(fd, ptr, nleft)) < 0) {
	    if(errno == EINTR)
		nread = 0;
	    else
		return (-1);
	} else if(nread == 0) {
	    break;
	}

	nleft -= nread;
	ptr += nread;
    }
    return size - nleft;
}

void read_data(int sockfd){
    ssize_t n;
    char buf[1024];

    int time = 0;
    for(;;) {
	fprintf(stdout, "block in read\n");
	if((n = readn(sockfd, buf, 1024)) == 0)
	    return;
	
	time ++;
	fprintf(stdout, "1K read for %d \n", time);
	usleep(1000);
    }
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVER_PORT);

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, 1024);

    for(;;) {
	clilen = sizeof(cliaddr);
	connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
	read_data(connfd);
	close(connfd);
    }
    return 0;
}
