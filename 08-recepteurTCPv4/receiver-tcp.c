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
#define QUEUE_LENGTH 1

int main (int argc, char *argv [])
{
    /* test arg number */
    if( argc != 3){
        fprintf(stderr, "usage: %s ip_addr port_number\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    /* convert and check port number */
    int port_number = atoi(argv[2]);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }

    char response[SIZE];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    
    /* create socket */
    int sockfd;
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sockfd);
    /* SO_REUSEADDR option allows re-starting the program without delay */
    int iSetOption = 1;
    CHECK (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &iSetOption,
		       sizeof iSetOption));


    struct sockaddr_storage ss;
    struct sockaddr *src_addr = (struct sockaddr *) &ss;
    socklen_t lenaddr = sizeof(*src_addr);

    struct addrinfo hints={0};
   // memset(&hints, 0, sizeof(hints));
    struct addrinfo *res = NULL;

    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV; // Eviter requête DNS
   
    int fd_addr = getaddrinfo (argv[1], argv[2], &hints, &res);
    if (fd_addr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd_addr));
        exit(EXIT_FAILURE);
    }

    if(res==NULL){
        exit(EXIT_FAILURE);
    }
    CHECK(bind (sockfd, res->ai_addr, res->ai_addrlen));
    CHECK(listen(sockfd, QUEUE_LENGTH)); // Ici taille de la socket est 1 car on suppose un seul client
    int socketr=accept(sockfd, res->ai_addr, &res->ai_addrlen);
    CHECK(socketr);

    CHECK(recv(socketr, response, SIZE, 0));
    CHECK(getnameinfo ((struct sockaddr *)src_addr,lenaddr, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST));
    
    printf("%s %s\n",host,serv);
    printf("%s",response);
    

    freeaddrinfo (res); 

    CHECK(close(sockfd));

    return 0;

    /* complete struct sockaddr */

    /* link socket to local IP and PORT */

    /* set queue limit for incoming connections */

    /* wait for incoming TCP connection */

    /* print sender addr and port */

    /* wait for incoming message */

    /* close sockets */

    /* free memory */

    /* print received message */

    return 0;
}
