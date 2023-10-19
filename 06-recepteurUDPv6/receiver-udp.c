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

#define IP   "::1"
#define SIZE 100

int main (int argc, char *argv [])
{
    // test nombres args et intervalles
    if( argc != 3){
        fprintf(stderr, "usage: %s ip_addr port_number\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    char * str_port = argv[2];

    int port_number = atoi(str_port);
    if(port_number<10000 || port_number > 65000){
        fprintf(stderr,"Mauvais format de numero de port : intervalle [10 000:65 000]\n");
        exit(EXIT_FAILURE);
    }

    // creation de socket
    int fd_socket;
    fd_socket=socket(PF_INET6, SOCK_DGRAM, 0);
    CHECK(fd_socket);


    struct addrinfo *res = NULL;

   

    int fd_addr = getaddrinfo (argv[1], argv[2], NULL, &res);
    if (fd_addr != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(fd_addr));
        exit(EXIT_FAILURE);
    }


    if(res==NULL){
        exit(EXIT_FAILURE);
    }
    else{
        CHECK(bind (fd_socket, res->ai_addr, res->ai_addrlen));

        char response[SIZE];
        memset(response,0,SIZE);
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        
        struct sockaddr_storage * src_addr = malloc(sizeof *src_addr);
        socklen_t lenaddr = sizeof *src_addr;
        int sizemsg=recvfrom(fd_socket, response, SIZE, 0, (struct sockaddr *)src_addr,&lenaddr);
        CHECK(sizemsg);
        CHECK(getnameinfo ((struct sockaddr *)src_addr, sizeof *src_addr, host, NI_MAXHOST, serv, NI_MAXSERV, NI_DGRAM|NI_NUMERICHOST));

        response[sizemsg]='\0';
        printf("%s",response);
        printf("%s %s\n",host,serv);

        freeaddrinfo (res); 
        free(src_addr);

    }

    CHECK(close(fd_socket));

    return 0;
}
