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

//#define PORT 61001
#define MAXDATASIZE 4096

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

void
getExpectedResult(
    char*                            testID,
    char*                            input,
    char*                            expected
    )
{
    if (!testID || !input || !expected)
    {
        return;
    }
    if (strcmp(testID, "TEST 1") == 0)
    {
        strcpy(input, "GET /v1/pkg HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\nTransfer-Encoding:chunked\r\n\r\n6\r\nThis i\r\n9\r\ns payload\r\n0\r\n");
        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:15\r\n\r\nThis is payload");
    }
    else if (strcmp(testID, "TEST 2") == 0)
    {
        strcpy(input, "GET /v1/pkg?x=y HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\nContent-Length: 15\r\n\r\nThis is payload");
        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:15\r\n\r\nThis is payload");
    }
    else if (strcmp(testID, "TEST 3") == 0)
    {
        strcpy(input, "GET /v1/pkg?cn=tom_077&cn=Users&dc=vsphere&dc=local HTTP/1.1\r\nAuthorization: Basic wqBjbj1BZG1pbmlzdHJhdG9yLGNuPVVzZXJzLGRjPXZzcGhlcmUsZGM9bG9jYWw6\r\nUser-Agent: curl/7.19.7 (x86_64-suse-linux-gnu) libcurl/7.19.7 OpenSSL/0.9.8j zlib/1.2.7 libidn/1.10\r\nHost: localhost\r\nAccept: */*\r\nContent-Length: 82\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n[    { \"op\": \"replace\", \"path\": \"description\", \"value\": \"My fourth Description\" }]");

        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:82\r\n\r\n[    { \"op\": \"replace\", \"path\": \"description\", \"value\": \"My fourth Description\" }]");
    }
    else if (strcmp(testID, "TEST 4") == 0)
    {
        strcpy(input, "DELETE /v1/pkg?cn=random5&cn=Users&dc=vsphere&dc=local HTTP/1.1\r\nAuthorization: Basic wqBjbj1BZG1pbmlzdHJhdG9yLGNuPVVzZXJzLGRjPXZzcGhlcmUsZGM9bG9jYWw6\r\nUser-Agent: curl/7.19.7 (x86_64-suse-linux-gnu) libcurl/7.19.7 OpenSSL/0.9.8j zlib/1.2.7 libidn/1.10\r\nHost: localhost\r\nAccept: */*\r\n\r\n");
        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 5") == 0)
    {
        strcpy(input, "GET /v1/pkg?x=y HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\nContent-Length: 20\r\n\r\nThis");
        strcpy(expected, "HTTP/1.1 408 Request Timeout\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 6") == 0)
    {
        strcpy(input, "Non HTTP Junk Data which should be discarded totally\r\n\r\ndfdfafdsfadsf");
        strcpy(expected, "HTTP/1.1 405 Method Not Allowed\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 7") == 0)
    {
        strcpy(input, "GET /v1/pkg?x=y HTTP/1.1\r\nHost: SITE\r\nConnection: Keep-Alive\r\n\r\nThis is payload");
        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 8") == 0)
    {
        strcpy(input, "GET /v1/pkg?x=y HTTP/1.1\r\nHost: SITE\r\nContent-Length: 25\r\n\r\nThis is payload with CRLF\r\n");
        strcpy(expected, "HTTP/1.1 200 OK\r\nConnection:close\r\nContent-Length:25\r\n\r\nThis is payload with CRLF");
    }
    else if (strcmp(testID, "TEST 9") == 0)
    {
        strcpy(input, "GET /v1/pkg?x=y HTTP/1.1\r\nHost: SITE\r\nContent-Length: 28\r\nTransfer-Encoding:chunked\r\n\r\nThis is payload with4\r\ndfdf\r\n");
        strcpy(expected, "HTTP/1.1 411 Length Required\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 10") == 0)
    {
        strcpy(input, " ");
        strcpy(expected, "HTTP/1.1 408 Request Timeout\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }
    else if (strcmp(testID, "TEST 11") == 0)
    {
        strcpy(input, "\r\n\r\b\r\n\r\n");
        strcpy(expected, "HTTP/1.1 405 Method Not Allowed\r\nConnection:close\r\nContent-Length:0\r\n\r\n");
    }


}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    char expected[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    int c = 0;

    if (argc != 4) {
        printf("Wrong number of arguments supplied to test program\n");
        exit(1);
    }

    memset(buf, '\0',MAXDATASIZE);
    memset(expected, '\0',MAXDATASIZE);

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

    getExpectedResult(argv[3],buf ,expected);

    write(sockfd, buf, strlen(buf));

    memset(buf, '\0',MAXDATASIZE);
    sleep(1);

 
    while(1)
    {
        int x = 0;
        x = read(sockfd, buf, MAXDATASIZE);
        if (x == 0){
            break;
        }
        if (strcmp(buf, expected) == 0)
        {
            printf("\n%s PASSED\n", argv[3]);
        }
        else
        {
            printf("%s",buf);
            printf("\n%s FAILED\n", argv[3]);
        }

        sleep(1);
        memset(buf, '0',MAXDATASIZE);
        c++;
    }
    close(sockfd);
    return 0;
}

