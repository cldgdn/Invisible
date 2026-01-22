#ifndef INVISIBLE_ROOM_H
#define INVISIBLE_ROOM_H
#include "TileMap.h"
#include "../dynamic/Bullet.h"
#include "../dynamic/Guard.h"
#include "../dynamic/Prop.h"


class Room {
public:
    Vec2 playerStartPos;
    TileMap *tileMap;
    std::vector<Guard*> guards;
    std::vector<Prop*> props;
    std::vector<Bullet*> bullets;

    Room(Vec2 playerStartPos, TileMap *tileMap);
    ~Room();
private:

};


#endif //INVISIBLE_ROOM_H