#include "Serializable.h"

#include <iostream>
#include <string>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class Jugador: public Serializable
{
private:
    int16_t pos_x;
    int16_t pos_y;

    static const size_t MAX_NAME = 20;

    char name[MAX_NAME];

public:
    Jugador(const char * _n, int16_t _x, int16_t _y): pos_x(_x), pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        _size = (sizeof(int16_t) * 2) + (sizeof(char) * MAX_NAME);
        alloc_data(_size);

        char* aux = _data;
        memcpy(aux, name, MAX_NAME * sizeof(char));
        aux += MAX_NAME * sizeof(char);
        memcpy(aux, &pos_x, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(aux, &pos_y, sizeof(int16_t));
        }

    int from_bin(char * data)
    {
        char* aux = data;
        memcpy(name, aux, MAX_NAME * sizeof(char));
        aux += MAX_NAME * sizeof(char);
        memcpy(&pos_x, aux, sizeof(int16_t));
        aux += sizeof(int16_t);
        memcpy(&pos_y, aux, sizeof(int16_t));

        return 0;
    }
	
    int16_t getX() const { return pos_x; }
    int16_t getY() const { return pos_y; }
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("./Player_ONE.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
    
    ssize_t bytes = write(fd, one_w.data(), one_w.size());
    if(bytes != one_w.size()) std::cout << "ERROR: info is different.\n";

    close(fd);

    return 0;
}