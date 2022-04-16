#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <thread>


const size_t MAXBUFFER = 80;
#define MAXTHREAD 5

class Msg{
    private:
        int sd;
        int id;
    public:
        Msg(int s, int i){
            sd = s;
            id = i;
        }
        void doMsg(){
            char tiempo[12];
            char buffer[MAXBUFFER];
            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            struct sockaddr_in cliente;
            socklen_t cliente_len = sizeof(struct sockaddr_in);

            while (true)
            {
                ssize_t bytes = recvfrom(sd, buffer, 79, 0, (struct sockaddr *) &cliente,
                        &cliente_len);
                
                if (bytes == -1) {
				std::cerr << "ERROR: no se reciben bytes\n";
	                	return;
			    }

                buffer[bytes]='\0'; 
                getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST,
                     serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
                
                std::cout << bytes << " bytes de " << host << ":" << serv << "\n";


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
                default:
                    {
                    std::printf("Comando no soportado: %s\n", buffer);
                    char error[21] = "Comando no soportado";
                    sendto(sd, error, sizeof(error), 0, (struct sockaddr*) &cliente, cliente_len);
                    break;
                    }
                }
            }
        }
        
            
    
};

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

    if(bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen)==-1){
        std::cerr << "ERROR: fallo en la asignacion a socker";
        return -1;
    }

	freeaddrinfo(result);

    for(int i = 0; i < MAXTHREAD; i++){
        Msg* mThr = new Msg(sd, i);
        std::thread([&mThr](){mThr->doMsg(); delete mThr; }).detach();
    }

    std::string q = "_";
    while (q!="q")
    {
        std::cin >> q;
    }
    close(sd);
    return 0;

}