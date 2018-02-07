/**************************************************************************
* This test client will open connection to server but will not send any
* data. This will make server to timeout and execute the timeout code
* path. Please refer README in the same folder for more information 
**************************************************************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

/************** EDIT THIS **************/

#define  SERVER_IP     "172.16.127.131"
#define  SERVER_PORT   "81"

/***************************************/


#define MAXDATASIZE 4096


int main(int argc, char *argv[])
{
    int sockfd = -1;
    int nBytes = 0;
    char buf[MAXDATASIZE] = {0};
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

again:
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        printf("client: failed to connect\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    nBytes = read(sockfd, buf, MAXDATASIZE);

    close(sockfd);
    goto again;

    return 0;

}
