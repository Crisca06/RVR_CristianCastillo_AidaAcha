#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string.h>

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE; //Devuelve 0.0.0.0 
    hints.ai_family = AF_UNSPEC; //Cualquier familia

    int rc = getaddrinfo(argv[1], argv[2], &hints, &result);

    //Si rc devuelve algo distinto de 0 ha habido un error
    if(rc != 0) {
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    for(struct addrinfo* r = result; r != nullptr; r = r->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        getnameinfo(r->ai_addr, r->ai_addrlen, host, NI_MAXHOST, serv, 
        NI_MAXSERV, NI_NUMERICHOST);
        std::cout << host << "\t" << r->ai_family << "\t" << r->ai_socktype << "\n";
    }
    return 0;
}