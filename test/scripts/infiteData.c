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


void
VmSockPosixSetSocketNonBlocking(
    int                              server_fd
    )
{
    int                              cur_flags = 0;
    int                              set_flags = 0;

    cur_flags = fcntl(server_fd, F_GETFL, 0);
    cur_flags |= O_NONBLOCK;
    set_flags = fcntl(server_fd, F_SETFL, cur_flags);

}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char *buf = NULL;
    char* res = NULL;
    char expected[512];
    char expected1[512];
    char out[512];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int c = 0, x = 0;
    int max_data_size = 0;

    if (argc != 5) {
        printf("Wrong number of arguments supplied to test program\n");
        exit(1);
    }

    max_data_size = atoi(argv[3]);

    buf = (char*)malloc(max_data_size);

    memset(buf, '\0',max_data_size);
    memset(expected, '\0', 512);
    memset(expected1, '\0', 512);
    memset(out, '\0', 512);
    strcpy(expected, "HTTP/1.1 400 Bad Request\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    strcpy(expected1, "HTTP/1.1 431 Large Header Field\r\nConnection:close\r\nContent-Length:0\r\n\r\n");

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
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

    res = expected;
    if (((strcmp(argv[4], "TEST 7") == 0) || (strcmp(argv[4], "TEST 8") == 0))) 
    {
        res = expected1;
    }

    memset(buf, 'B', max_data_size);
    write(sockfd, buf, max_data_size);
    x = read(sockfd, out, 512);

    sleep(1);

    if (strcmp(out, res) == 0)
    {
        printf("\n%s PASSED\n", argv[4]);
    }
    else
    {
        printf("\n%s FAILED\n", argv[4]);
    }

    close(sockfd);
    free(buf);
    return 0;
}

