#include "Room.h"

Room::Room(TileMap *tileMap, std::vector<Guard *> *guards) : tileMap(tileMap), guards(guards) {
}

Room::~Room() {
    delete tileMap;
    for (Guard *g : *guards) {
        delete g;
    }
    guards->clear();
    delete guards;
}
