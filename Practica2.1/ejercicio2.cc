#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    int sd = socket(result->ai_family, result->ai_socktype, 0);

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    while (1) {
        char buffer[80];
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        struct sockaddr_in cliente;
        socklen_t cliente_len = sizeof(struct sockaddr_in);

        ssize_t bytes = recvfrom(sd, buffer, 79, 0, (struct sockaddr *) &cliente,
                        &cliente_len);
        buffer[bytes]='\0'; 
        
        getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST,
                     serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        printf("Conexión desde Host:%s Puerto:%s\n",host, serv);
        printf("\tMensaje(%ld): %s\n", bytes, buffer);

        sendto(sd, buffer, bytes, 0, (struct sockaddr *) &cliente, cliente_len);
    }

}