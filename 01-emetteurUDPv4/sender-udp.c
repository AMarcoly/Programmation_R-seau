#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define CHECK(op)   do { if ( (op) == -1) { perror (#op); exit (EXIT_FAILURE); } \
                    } while (0)

#define IP      0x100007f /* 127.0.0.1 */
#define PORT(p) htons(p)

int main (int argc, char *argv [])
{
   if( argc != 2){
        fprintf(stderr, "Erreur us : %s <port number> \n",argv[0]);
        exit(EXIT_FAILURE);
    }

    int port_number = atoi(argv[1]);
    if(port_number < 10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }

    int socket_fd;
    char* msg = "hello world";
    socket_fd = socket (AF_INET, SOCK_DGRAM, 0);
    CHECK(socket_fd);


    struct sockaddr_storage ss;
    struct sockaddr_in *in = (struct sockaddr_in *) &ss;


    in -> sin_family = AF_INET;
    in-> sin_port =PORT(port_number);
    in -> sin_addr.s_addr  = IP; 


    CHECK(sendto(socket_fd,msg,strlen(msg),0,(struct sockaddr *)in,sizeof (*in)));

  
    CHECK(close(socket_fd));

    return 0;
}
