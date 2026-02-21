#include "Room.h"
#include "../dynamic/RoomExit.h"
#include "../dynamic/BoxPickup.h"

Room::Room(Vec2 playerStartPos, TileMap *tileMap, bool isDark) :
    playerStartPos(playerStartPos), tileMap(tileMap), isDark(isDark), exit(nullptr), box(nullptr) {}

Room::~Room() {
    delete tileMap;
    delete exit;
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
