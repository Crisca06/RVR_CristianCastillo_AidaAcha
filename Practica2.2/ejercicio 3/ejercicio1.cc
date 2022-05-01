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

public:
    char name[MAX_NAME];

    Jugador(const char * _n, int16_t _x, int16_t _y): pos_x(_x), pos_y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    //Copiamos el nombre a partir de la dirección marcada por aux,
    //avanzamos hasta pasar dicho nombre y copiamos la posición x a
    //partir de la direccion nueva de aux, y se hace lo mismo con la posición y.
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

    //Repetimos el mismo proceso copiando el archivo en la dirección data.
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
    // 2. Escribir la serialización en un fichero
    // 3. Leer el fichero
    // 4. "Deserializar" en one_r
    // 5. Mostrar el contenido de one_r

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("./Player_ONE.txt", O_CREAT | O_TRUNC | O_RDWR, 0666);
    
    ssize_t bytes = write(fd, one_w.data(), one_w.size());
    if(bytes != one_w.size()) std::cout << "ERROR: info is different.\n";

    close(fd);

    // 3. Leer el fichero
    fd = open("./Player_ONE.txt", O_RDONLY, 0666);
    char buffer[bytes];

    if (read(fd, &buffer, bytes) == -1) {
	std::cerr << "ERROR: an error ocurred while reading file.\n";
	return -1;
    }

    close(fd);

    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);

    // 5. Mostrar el contenido de one_r
    std::cout << "Nombre del jugador: " << one_r.name << " - Pos X: " << one_r.getX() << " - Pos Y: " << one_r.getY() << "\n";

    return 0;
}