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

std::vector<Vec2 *>* Pathfinder::findPath(Room *room, Vec2 *start, Vec2 *dest) {
    int destx, desty, startx, starty;
    PathNode *currentNode;
    bool pathFound = false;
    std::vector<Vec2 *>* path = new std::vector<Vec2 *>();

    destx = ((dest->x - std::floor(dest->x)) >= 0.5f) ? (int) std::floor(dest->x) + 1 : (int) std::floor(dest->x);
    desty = ((dest->y - std::floor(dest->y)) >= 0.5f) ? (int) std::floor(dest->y) + 1 : (int) std::floor(dest->y);
    startx = ((start->x - std::floor(start->x)) >= 0.5f) ? (int) std::floor(start->x) + 1 : (int) std::floor(start->x);
    starty = ((start->y - std::floor(start->y)) >= 0.5f) ? (int) std::floor(start->y) + 1 : (int) std::floor(start->y);

    if (room == nullptr) {
        return nullptr;
    }

    if (room != this->room) {
        this->room = room;
        setupNodes(destx, desty);
    }

    nodes[starty][startx]->gcost = 0;
    frontier.push(nodes[starty][startx]);

    while (!frontier.empty()) {
        currentNode = frontier.top();
        frontier.pop();

        if (currentNode->explored) {
            continue;
        }
        if (currentNode->x == destx && currentNode->y == desty) {
            pathFound = true;
            break;
        }

        explore(currentNode->x, currentNode->y);
    }

    if (pathFound) {
        currentNode = nodes[desty][destx];
        currentNode->next = nullptr;

        while (currentNode != nodes[starty][startx]) {
            currentNode->prev->next = currentNode;
            currentNode = currentNode->prev;
        }
        currentNode = nodes[starty][startx];

        while (currentNode != nullptr) {
            path->push_back(new Vec2{(float) currentNode->x, (float) currentNode->y});

            currentNode = currentNode->next;
        }

        return path;
    }

    return nullptr;
}

void Pathfinder::setupNodes(int destx, int desty) {
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            if (nodes[i][j] != nullptr) delete nodes[i][j];

            if (!(this->room->tileMap->tiles[i][j] & TileMap::SOLID_BIT)) {
                nodes[i][j] = new PathNode{j, i, heuristic(j, i, destx, desty), 10000000000.0f, false, nullptr};
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
    unsigned char (*tiles)[ROOM_WIDTH] = room->tileMap->tiles;
    bool admissible, diagonal;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            //check if in bounds
            if (i == 0 && j == 0) continue;
            if (nodex + j < 0 || nodex + j >= ROOM_WIDTH) continue;
            if (nodey + i < 0 || nodey + i >= ROOM_HEIGHT) continue;

            admissible = false;
            diagonal = false;

            //diagonal step logic
            if (abs(i) == abs(j)) {
                diagonal = true;
                if (
                    !(tiles[nodey + i][nodex] & TileMap::SOLID_BIT) &&
                    !(tiles[nodey][nodex + j] & TileMap::SOLID_BIT) &&
                    !(tiles[nodey + i][nodex + j] & TileMap::SOLID_BIT) &&
                    nodes[nodey + i][nodex + j] != nullptr && !nodes[nodey + i][nodex + j]->explored
                )
                    admissible = true;
            }
            else
                admissible = nodes[nodey + i][nodex + j] != nullptr && !nodes[nodey + i][nodex + j]->explored;

            if (admissible) {
                float gcost = nodes[nodey][nodex]->gcost + (diagonal ? 1.4f : 1.0f);
                if (gcost < nodes[nodey + i][nodex + j]->gcost) {
                    nodes[nodey + i][nodex + j]->gcost = gcost;
                    nodes[nodey + i][nodex + j]->prev = nodes[nodey][nodex];
                    frontier.push(nodes[nodey + i][nodex + j]);
                }
            }
        }
    }

    nodes[nodey][nodex]->explored = true;
}
