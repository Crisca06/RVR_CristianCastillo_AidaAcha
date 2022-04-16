#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <thread>


const size_t MAXBUFFER = 80;
#define MAXLISTEN 3

std::condition_variable numCv;
std::mutex numMutex;
int numListen = 0;

class MyClassThread{
    private:
        int cliente_sd;
        int id;
    public:
        MyClassThread(int s, int i)
        {
            cliente_sd = s;
            id = i;
        };

        void connect(){
                int i = 0; ssize_t c; bool connected = true;

            while(connected){
                char buffer[80]; 

                do {
                    c = recv(cliente_sd, &buffer[i], 1, 0);
                    if(c < 1) connected = false;
                } while ( c >= 0 && i < 79 && buffer[i++] != '\n');
            
                if(connected){
                    buffer[i] = '\0';	
                    send(cliente_sd, buffer, i, 0);
                    i = 0;
                } else 
                    std::cout << "Conexion terminada. \n";
            }
            close(cliente_sd);

            numMutex.lock();
            numListen--;
            if(numListen < MAXLISTEN){
                numCv.notify_all();
            }
            numMutex.unlock();
        }
};

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

    bind(sd, (struct sockaddr *) result->ai_addr, result->ai_addrlen);
    listen(sd, 5);

	freeaddrinfo(result);

    while (1) {
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        struct sockaddr cliente;
        socklen_t cliente_len = sizeof(struct sockaddr);

        {
            std::unique_lock<std::mutex> lck(numMutex);
            while (numListen >= MAXLISTEN)
            {
                std::cout << "El servidor esta lleno, espere...";
                numCv.wait(lck);
            }
            
        }

        int cliente_sd = accept(sd, (struct sockaddr *) &cliente, &cliente_len);

        if(cliente_sd == -1){
            std::cerr <<"ERROR: fallo en accept\n";
            close(sd);
            return -1;
        }

        getnameinfo((struct sockaddr *) &cliente, cliente_len, host, 
                    NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        
        std::cout << "Conexion desde " << host <<  ":" << serv << "\n";

        numMutex.lock();
        numListen++;
        numMutex.unlock();

        MyClassThread* mThr = new MyClassThread(cliente_sd, numListen);
        std::thread([&mThr]() {mThr->connect(); delete mThr;}).detach();

        close(sd);
        return 0;
    }
}