#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP   "::1"
#define SIZE 100

void cpy (int src, int dst)
{
    char buffer[SIZE];
    ssize_t bytes_read;
    int n;

    while ((bytes_read = read(src, buffer, SIZE)) > 0)
    {
        n=write(dst, buffer, bytes_read);
        CHECK(n);
    }
    CHECK(bytes_read);
}

int main (int argc, char *argv [])
{
     /* test arg number */
    if( argc != 4){
        fprintf(stderr, "usage: %s ip_addr port_number filename\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    /* convert and check port number */
    int port_number = atoi(argv[2]);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }
    /* open file to send */
    int fd = open(argv[3],O_RDONLY);
    CHECK(fd);
    /* create socket */
    int sockfd;
    sockfd=socket(AF_INET6, SOCK_STREAM, 0);
    CHECK(sockfd);
    /* complete struct sockaddr */
    
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *res = NULL;

    hints.ai_family = AF_INET6; // IPV6
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV; 

    int fd_addr = getaddrinfo (argv[1], argv[2], &hints, &res);
    if (fd_addr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd_addr));
        exit(EXIT_FAILURE);
    }

    if(res==NULL){
        exit(EXIT_FAILURE);
    }else
    {
        /* connect to the remote peer */
        CHECK(connect( sockfd,res->ai_addr,res->ai_addrlen));
        /* send the file content */
        cpy(fd, sockfd);
        /* close socket */
        CHECK(close(sockfd));
        /* close file */
        CHECK(close(fd));
        /* free memory */
        freeaddrinfo(res);
    }
    return 0;
}
