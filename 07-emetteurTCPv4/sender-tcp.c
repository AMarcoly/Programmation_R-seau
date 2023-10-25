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

#define IP "127.0.0.1"

int main (int argc, char *argv [])
{
    /* test arg number */
    if( argc != 3){
        fprintf(stderr, "usage: %s ip_addr port_number\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    char * str_port = argv[2];
    const char * message = "hello world";

    /* convert and check port number */
    int port_number = atoi(str_port);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }
    /* create socket */
    int fd_socket;
    fd_socket=socket(AF_INET, SOCK_STREAM, 0);
    CHECK(fd_socket);

  
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *res = NULL;

    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM; //TCP

    int fd_addr = getaddrinfo (argv[1], str_port, &hints, &res);
    if (fd_addr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd_addr));
        exit(EXIT_FAILURE);
    }

    if(res==NULL){
        exit(EXIT_FAILURE);
    }
    else{
        /* connect to server */
        CHECK(connect(fd_socket,res->ai_addr,res->ai_addrlen));
        /* send message */
        CHECK(send(fd_socket, message, strlen(message), 0));
        freeaddrinfo (res);    
    }

    CHECK(close(fd_socket));
    return 0;
}
