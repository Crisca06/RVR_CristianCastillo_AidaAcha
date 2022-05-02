#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <iostream>
#include <stdexcept>

#include <ostream>

class Serializable;

/*
  Clase que representa el extremo local de una conexión UDP. Inicializa un sockects
   y descripción binaria del extremo (dirección IP y puerto)
 */

class Socket
{
public:

    //Utilizamos esta constante para definir buffers de recepción.
    //El máximo teórico de un mensaje UDP es 2^16, del que hay que descontar
    // la cabecera UDP e IP.

    static const int32_t MAX_MESSAGE_SIZE = 32768;

    //Construye el socket UDP con una dirección y puerto.

    Socket(const char * address, const char * port);

    //Inicializa un Socket copiando los parámetros del socket

    Socket(struct sockaddr * _sa, socklen_t _sa_len);

    virtual ~Socket() {};

    int recv(Serializable &obj, Socket * &sock);

    int recv(Serializable &obj); //Descarta los datos del otro extremo

    int send(Serializable& obj, const Socket& sock);

    int bind();

    friend std::ostream& operator<<(std::ostream& os, const Socket& dt);

    friend bool operator== (const Socket &s1, const Socket &s2);
    friend bool operator!= (const Socket &s1, const Socket &s2);

protected:
    int sd;

    struct sockaddr sa;

    socklen_t sa_len;

}

#endif /* SOCKET_H_ */