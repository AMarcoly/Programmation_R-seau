#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define SIZE 100

void cpy (int src, int dst)
{
    char buffer[SIZE];
    ssize_t bytes_read, bytes_written;

    while ((bytes_read = read(src, buffer, SIZE)) > 0)
    {
        bytes_written = write(dst, buffer, bytes_read);
        CHECK(bytes_written);
    }
    CHECK(bytes_read);
}

int main (int argc, char * argv[])
{
    /* test arg number */
    if (argc != 2) {
        fprintf(stderr, "usage: %s server_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* get the list of struct addrinfo */
    struct addrinfo hints={0};
    struct addrinfo *result = NULL;
    struct addrinfo *iter = NULL;

    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* TCP socket */
    hints.ai_flags = 0;
    hints.ai_protocol = 0;           /* Any protocol */

    int fd_addr = getaddrinfo(argv[1], "http", &hints, &result);
    if (fd_addr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd_addr));
        exit(EXIT_FAILURE);
    }

    /* create socket */
    int sockfd;
    iter = result;


    CHECK(sockfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol));
    
    // hmm 
    if(iter == NULL){
        exit(EXIT_SUCCESS); // test 4 sortie sans prob
    }

    /* connect to the server */
    CHECK(connect(sockfd, iter->ai_addr, iter->ai_addrlen));

    /* prepare GET cmd */
    char get_cmd[SIZE];
    snprintf(get_cmd, SIZE, "GET / HTTP/1.1\r\nHost: %s\r\nConnection:close\r\n\r\n", argv[1]);

    /* send GET cmd and get the response */
    CHECK(send(sockfd, get_cmd, strlen(get_cmd), 0));

    cpy(sockfd, STDOUT_FILENO);

    /* close socket */
    close(sockfd);

    /* free struct addrinfo list */
    freeaddrinfo(result);
    return 0;
}
