#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define PORT(p) htons(p)

#define MAX_SIZE 256

int main (int argc, char *argv [])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s port_number\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* convert and check port number */
    int port_number = atoi(argv[1]);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000: \
        65 000]\n");
        exit(EXIT_FAILURE);
    }

    // Data
    char * msg = "/HELO";
    char * quitter = "/QUIT";
    char recv_buffer[MAX_SIZE];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

        
    ssize_t bytes_received;

    /* create socket */
    int sockfd;
    CHECK(sockfd=socket(AF_INET6, SOCK_DGRAM, 0));

    /* set dual stack socket */
    int value = 0;
    CHECK(setsockopt (sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof value));

    /* set local addr */
    struct sockaddr_in6 ss={0};
    ss.sin6_family = AF_INET6;
    ss.sin6_port = PORT(atoi(argv[1]));
    ss.sin6_addr = in6addr_any;
    socklen_t len_ss = sizeof(ss);

    /* check if a client is already present */
    int retour;
    retour = bind(sockfd, (struct sockaddr*)&ss, len_ss) ;
    if (retour == -1) {
        if (errno == EADDRINUSE) {
            // si un client est sur le port, on lui envoie /HELO
            CHECK(sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&ss, sizeof ss));
        } else {
            perror("bind");
            exit(EXIT_FAILURE);
        }
    } else {
        // pas de client sur le port et le bind a réussi, attend un /HELO
        CHECK(bytes_received = recvfrom(sockfd, recv_buffer, MAX_SIZE, 0,
                (struct sockaddr*)&ss, &len_ss));

        // Traitement du message reçu
        if (strcmp(recv_buffer, "/HELO") == 0) {
            CHECK(getnameinfo((struct sockaddr *)&ss, sizeof(ss), host,        \
            NI_MAXHOST,serv, NI_MAXSERV, NI_DGRAM|NI_NUMERICHOST));
            // printf("host");
            printf("%s %s\n", host, serv);
            
        }
    }
    
    /* prepare struct pollfd with stdin and socket for incoming data */
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    char buffer[MAX_SIZE];

   
    /* main loop */
    while (1) {
        CHECK(poll(fds, 2, -1)); 

        if (fds[0].revents & POLLIN) { // dans l'entrée standard
            // Je récupère les données écrites
            fgets(buffer, MAX_SIZE, stdin);
       
            if(strcmp(buffer, "/HELO\n") == 0) {
                printf("Commande /HELO reçue. Adresse distante\n");
            } 
            else if (strcmp(buffer, "/QUIT\n") == 0)
            {
               CHECK(sendto(sockfd, quitter, strlen(quitter), 0, (struct sockaddr*)&ss, sizeof ss)); 
            }
            // Action: send data to the other client

        }

        if (fds[1].revents & POLLIN) {
            // récupérer data du socket
            // Recevoir un message et le traiter
             CHECK(bytes_received = recvfrom(sockfd, recv_buffer, MAX_SIZE, 0,
                (struct sockaddr*)&ss, &len_ss));
            // Traitement du message reçu
            if (strcmp(recv_buffer, "/QUIT") == 0) {
                // printf("Quit fds1 while\n");
                CHECK(sendto(sockfd, quitter, strlen(quitter), 0, (struct sockaddr*)&ss, sizeof ss));
                break;
            }
            // ...
            // Event: recv data Action: process data
        }
    }

    /* close socket */
    CHECK(close(sockfd));

    /* free memory */
    

    return 0;
}
