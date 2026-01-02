#include "Pathfinder.h"

Pathfinder::Pathfinder() : room(nullptr) {
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            nodes[i][j] = nullptr;
        }
    }
}

Pathfinder::~Pathfinder() {
}

std::vector<Vec2 *> Pathfinder::findPath(Room *room, Vec2 start, Vec2 dest) {
    if (room != this->room) {
        this->room = room;
        setupNodes();
    }
}

void Pathfinder::setupNodes() {
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            if (nodes[i][j] != nullptr) delete nodes[i][j];

            if (!(this->room->tileMap->tiles[i][j] & TileMap::SOLID_BIT)) {
                nodes[i][j] = new PathNode{0, 0, false, nullptr};
            }
            else {
                nodes[i][j] = nullptr;
            }
        }
    }
}

float Pathfinder::heuristic(int nodex, int nodey, int destx, int desty) {
    return abs(nodex - destx) + abs(nodey - desty);
}

void Pathfinder::explore(int nodex, int nodey) {
}
