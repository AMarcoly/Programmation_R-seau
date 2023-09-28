#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP   "127.0.0.1"

int main (int argc, char *argv [])
{
    //* test arg number */
//    if( argc != 2){
//         fprintf(stderr, "Erreur us : %s <port number> \n",argv[0]);
//         exit(EXIT_FAILURE);
//     }

//     /* convert and check port number */
//     int port_number = atoi(argv[1]);
//     if(port_number<10000 || port_number > 65000){
//         fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
//         exit(EXIT_FAILURE);
//     }

    /* create socket */

    /* complete sockaddr struct */

    /* send message to remote peer */

    /* close socket */

    /* free memory */

    return 0;
}
