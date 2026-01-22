#include "Room.h"

Room::Room(Vec2 playerStartPos, TileMap *tileMap) : playerStartPos(playerStartPos), tileMap(tileMap) {
}

Room::~Room() {
    delete tileMap;
    for (Guard *g : guards) {
        delete g;
    }
    guards.clear();
    for (Prop *prop : props) {
        delete prop;
    }
    props.clear();
    for (Bullet *bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
}
