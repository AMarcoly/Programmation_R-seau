#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define CHECK(op)               \
    do                          \
    {                           \
        if ((op) == -1)         \
        {                       \
            perror(#op);        \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

#define PORT(p) htons(p)

#define MAX_SIZE 256

#ifdef BIN
#define H 0x01
#define Q 0x02

struct messageB
{
    uint8_t msgB;
};
#endif

void clean_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s port_number\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* convert and check port number */
    int port_number = atoi(argv[1]);
    if (port_number < 10000 || port_number > 65000)
    {
        fprintf(stderr, "Mauvais format de numero de port : intervalle [10 000: \
        65 000]\n");
        exit(EXIT_FAILURE);
    }

    // Data
    char *msg = "/HELO";
    char recv_buffer[MAX_SIZE];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    ssize_t bytes_received;

#ifdef BIN
    struct messageB messageBinaire; // Prq je mettrai un pointeur?
#endif

    /* set local addr */
    struct sockaddr_in6 ss = {0};
    ss.sin6_family = AF_INET6;
    ss.sin6_port = PORT(atoi(argv[1]));
    ss.sin6_addr = in6addr_any;
    socklen_t len_ss = sizeof(ss);

    /* create socket */
    int sockfd;
    CHECK(sockfd = socket(AF_INET6, SOCK_DGRAM, 0));

    /* set dual stack socket */
    int value = 0;
    CHECK(setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof value));

    /* check if a client is already present */
    int retour;
    retour = bind(sockfd, (struct sockaddr *)&ss, len_ss);
    if (retour == -1)
    {
        if (errno == EADDRINUSE)
        {
#ifdef BIN
            // si un client est sur le port, on lui envoie /H qui est 0x01
            messageBinaire.msgB = H;
            CHECK(sendto(sockfd, &messageBinaire, sizeof(messageBinaire), 0, (struct sockaddr *)&ss, sizeof ss));
            // printf("send binaire\n");
#else
            // si un client est sur le port, on lui envoie /HELO
            // envoie message sur meme adresse ecoute,  à verif
            CHECK(sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&ss, sizeof ss));
#endif
        }
        else
        {
            perror("bind");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        // pas de client sur le port et le bind a réussi, attend un /HELO
        CHECK(bytes_received = recvfrom(sockfd, recv_buffer, MAX_SIZE, 0,
                                        (struct sockaddr *)&ss, &len_ss));
#ifdef BIN
        for (int i = 0; i < bytes_received; i++)
        {
            // printf("%x", recv_buffer[i]);
            if (recv_buffer[i] == H)
            {
                // printf("HELO");
                CHECK(getnameinfo((struct sockaddr *)&ss, sizeof(ss), host,
                                  NI_MAXHOST, serv, NI_MAXSERV, NI_DGRAM | NI_NUMERICHOST));
                printf("%s %s\n", host, serv);
            }
        }
#else
        // Traitement du message reçu
        if (strncmp(recv_buffer, "/HELO", 5) == 0)
        {
            CHECK(getnameinfo((struct sockaddr *)&ss, sizeof(ss), host,
                              NI_MAXHOST, serv, NI_MAXSERV, NI_DGRAM | NI_NUMERICHOST));
            // printf("%s\n", recv_buffer);
            printf("%s %s\n", host, serv);
        }
#endif
    }

    /* prepare struct pollfd with stdin and socket for incoming data */
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    char buffer[MAX_SIZE] = {0};

    /* main loop */
    int run = 1;
    int taille_buffer = 0;

    while (run)
    {
        CHECK(poll(fds, 2, -1));

        if (fds[0].revents & POLLIN)
        { // dans l'entrée standard
            // Je récupère les données écrites
            fgets(buffer, MAX_SIZE, stdin);
            if (strncmp(buffer, "/QUIT", 5) == 0)
            {
#ifdef BIN
                // printf("QUIT\n");
                messageBinaire.msgB = Q;
                CHECK(sendto(sockfd, &messageBinaire, sizeof(messageBinaire), 0, (struct sockaddr *)&ss, sizeof ss));
                run = 0;
#else
                CHECK(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&ss, sizeof ss));
                run = 0;
#endif
            }
            else
            {
                taille_buffer = strlen(buffer);
                buffer[taille_buffer - 1] = '\0';
                CHECK(sendto(sockfd, buffer, taille_buffer, 0, (struct sockaddr *)&ss, sizeof ss));
            }
        }

        if (fds[1].revents & POLLIN)
        {
            // récupérer data du socket
            // Recevoir un message et le traiter
            CHECK(bytes_received = recvfrom(sockfd, recv_buffer, MAX_SIZE, 0,
                                            (struct sockaddr *)&ss, &len_ss));
#ifdef BIN
            int i = 0;
            if (recv_buffer[i] == Q)
                run = 0;
#else
            // Traitement du message reçu
            if (strncmp(recv_buffer, "/QUIT", 5) == 0)
                run = 0;
#endif
            else
                printf("%s\n", recv_buffer);
        }
    }

    /* close socket */
    CHECK(close(sockfd));

    return 0;
}
