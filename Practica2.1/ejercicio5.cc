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
    hints.ai_socktype = SOCK_STREAM;

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

    int server = connect(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);

    if(server == -1){
        std::cerr << "ERROR al conectar al servidor.\n";
		return -1;
    }

	freeaddrinfo(result);

    char buffer[80];
    int i = 0; bool connected = true;

    while(connected){
        std::cin >> buffer;
        send(server, buffer, sizeof(buffer), 0);
        recv(server, buffer, sizeof(buffer), 0);
        
        if(buffer[1] == '\0' && buffer[0] == 'Q') {
            connected = false;
        } else {
    	    std::cout << buffer << "\n";
            buffer[0] = '\0';
        }
    }

    close(server);
    close(sd);
    return 0;
}