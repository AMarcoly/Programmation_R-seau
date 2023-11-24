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

int main (int argc, char *argv [])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s server_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* convert and check port number */
    int port_number = atoi(argv[2]);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }

    /* create socket */
    int sockfd;
    CHECK(sockfd=socket(AF_INET6, SOCK_DGRAM, 0));
    /* set dual stack socket */
    int value = 0;
    CHECK (setsockopt (sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof value));


    /* set local addr */

    /* check if a client is already present */

    /* prepare struct pollfd with stdin and socket for incoming data */

    /* main loop */

    /* close socket */
    CHECK(close(sockfd));

    /* free memory */

    return 0;
}
