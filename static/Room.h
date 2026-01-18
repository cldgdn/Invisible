#ifndef INVISIBLE_ROOM_H
#define INVISIBLE_ROOM_H
#include "TileMap.h"
#include "../dynamic/Bullet.h"
#include "../dynamic/Guard.h"


class Room {
public:
    TileMap *tileMap;
    std::vector<Guard*> *guards;
    std::vector<Bullet*> bullets;

    Room(TileMap *tileMap, std::vector<Guard*> *guards);
    ~Room();
private:

};


#endif //INVISIBLE_ROOM_H