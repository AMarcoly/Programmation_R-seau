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

#define IP   "127.0.0.1"
#define SIZE 100

int main (int argc, char *argv [])
{
    // test nombres args et intervalles
    if( argc != 2){
        fprintf(stderr, "Erreur us : %s <port number>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    char * str_port = argv[1];
    char response[SIZE];

    int port_number = atoi(str_port);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }

    // creation de socket
    int fd_socket;
    fd_socket=socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(fd_socket);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo *res = NULL;

    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_DGRAM; // UDP

    int fd_addr = getaddrinfo (IP, str_port, &hints, &res);
    CHECK(fd_addr);

    if(res==NULL){
        exit(EXIT_FAILURE);
    }
    else{
        CHECK(bind (fd_socket, res->ai_addr, res->ai_addrlen));

        CHECK(recvfrom(fd_socket, response, sizeof(response), 0, NULL,NULL));

        freeaddrinfo (res); 
        printf("%s",response);


    }

    CHECK(close(fd_socket));

    return 0;
}
