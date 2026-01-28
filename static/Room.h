#ifndef INVISIBLE_ROOM_H
#define INVISIBLE_ROOM_H
#include "TileMap.h"
#include "../dynamic/Bullet.h"
#include "../dynamic/Guard.h"
#include "../dynamic/Prop.h"

class RoomExit;

class Room {
public:
    Vec2 playerStartPos;
    TileMap *tileMap;
    std::vector<Guard*> guards;
    std::vector<Prop*> props;
    std::vector<Bullet*> bullets;
    bool isDark;
    RoomExit *exit;

    Room(Vec2 playerStartPos, TileMap *tileMap, bool isDark);
    ~Room();
};


#endif //INVISIBLE_ROOM_H