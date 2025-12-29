#ifndef INVISIBLE_ROOM_H
#define INVISIBLE_ROOM_H
#include "TileMap.h"
#include "../dynamic/Guard.h"


class Room {
public:
    TileMap *tileMap;
    std::vector<Guard*> guards;

private:


};


#endif //INVISIBLE_ROOM_H