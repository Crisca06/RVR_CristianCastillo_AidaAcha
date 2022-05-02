#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    
    struct addrinfo h;
    struct addrinfo*  r;

    memset((void *) &h, 0, sizeof(struct addrinfo));

    //UDP
    h.ai_family = AF_INET;
    h.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(address, port, &h, r);
    if (rc != 0)
	fprintf(stderr, "ERROR: [getaddrinfo]: " << gai_strerror(rc) << "\n";

    sd = socke r->ai_family r->ai_socktype, 0);
    if (sd == -1)
	std::cerr << "ERROR: [socket] no creado\n";

    sa = r->ai_addr;
    sa_len  r->ai_addrlen;

    freeaddrinf r);
}

Socket::Socket(struct sockaddr* _sa, socklen_t _sa_len) : sd(-1), sa(*_sa), sa_len(_sa_len) {
    sd = socket(sa.sa_family, SOCK_DGRAM, 0);
    bind();
}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::recv(Serializable &obj) 
{
    Socket * s = 0;

    return recv(obj, s);
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    obj.to_bin();

    ssize_t bytes = sendto(sock.sd, obj.data(), obj.size(), 0, &sock, sa, sock.sa_len);
    if (bytes < 1) {
	std::cerr << "ERROR: fallo al enviar mensaje\n";
	return -1;
    }
    return 0;
}

int Socket::bind()
{
    return ::bind(sd, (const struct sockaddr *) &sa, sa_len);
}

bool Socket::operator== (const Socket &s1, const Socket &s2)
{
    struct sockaddr_in* s1_in = (struct sockaddr_in *) &(s1.sa);
    struct sockaddr_in* s2_in = (struct sockaddr_in *) &(s2.sa);

    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    //Retornar false si alguno difiere
    if (s1.sa.sa_family != s2.sa.sa_family || s1_in->sin_addr.s_addr != s2_in->sin_addr.s_addr ||
	s1_in->sin_port != s2_in->sin_port)
	return false;
    else return true;
};

bool Socket::operator!= (const Socket &sock1, const Socket &sock2) {
    return !(sock1 == sock2);
};

std::ostream& Socket::operator<<(std::ostream& ent, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    ent << "Dir ip: "<< host << ", Puerto: " << serv;

    return ent;
};