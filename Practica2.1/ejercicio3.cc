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
        std::cerr << "ERROR al crear el socket.\n";
		return -1;
    }

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

	freeaddrinfo(result);

    char buffer[80];
    int s = sendto(sd, argv[3], strlen(argv[3]) + 1, 0, result->ai_addr, result->ai_addrlen);

	if (s == -1) {
		std::cerr << "ERROR en el sendto\n";
		close(sd); return -1;
	}

    int r = recvfrom(sd, buffer, 79, 0, result->ai_addr, &result->ai_addrlen);
    
	if (r == -1) {
		std::cerr << "ERROR en el recvfrom\n";      
		close(sd); return -1;
    }

	std::cout << buffer << "\n";

    close(sd);
    return 0;
}