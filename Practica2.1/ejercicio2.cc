#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
    hints.ai_family   = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);
    int sd = socket(result->ai_family, result->ai_socktype, 0);

    //Si rc devuelve algo distinto de 0 ha habido un error
    if(rc != 0) {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    if(sd == -1){
        std::cerr << "ERROR al crear el [socket].\n";
		return -1;
    }

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

	freeaddrinfo(result);

    bool connected = true;
    while (connected) {
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

        std::cout << bytes << " bytes de " << host << ":" << serv << "\n";

        char tiempo[12];

        switch (buffer[0])
        {
            case 't':
                {
                time_t tim;
                time(&tim);
                tm* tiemp = localtime(&tim);
                strftime(tiempo, sizeof(tiempo), "%r", tiemp);
                sendto(sd, tiempo, sizeof(tiempo), 0, (struct sockaddr *) &cliente, cliente_len);
                break;
                }
            case 'd':
                {
                time_t tim2;
                time(&tim2);
                tm* tiemp2 = localtime(&tim2);
                strftime(tiempo, sizeof(tiempo), "%F", tiemp2);
                sendto(sd, tiempo, sizeof(tiempo), 0, (struct sockaddr *) &cliente, cliente_len);
                break;
                }
            case 'q':
                {
                std::printf("Saliendo...");
                connected = false;
                char exit[12] = "Saliendo...";
                sendto(sd, exit, sizeof(exit), 0, (struct sockaddr*) &cliente, cliente_len);
                break;
                }
            default:
                {
                std::printf("Comando no soportado: %s\n", buffer);
                char error[21] = "Comando no soportado";
                sendto(sd, error, sizeof(error), 0, (struct sockaddr*) &cliente, cliente_len);
                break;
                }
        }
    
    }

    close(sd);
    return 0;
}