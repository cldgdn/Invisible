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

/*
 * Standard A* pathfinding implementation. Caches information about the last room used to pathfind in as to not reallocate the map each time.
 */
std::vector<Vec2 *>* Pathfinder::findPath(Room *room, Vec2 *start, Vec2 *dest) {
    int destx, desty, startx, starty;
    float tdestx, tdesty, tstartx, tstarty;
    PathNode *currentNode;
    bool pathFound = false;
    std::vector<Vec2 *>* path = new std::vector<Vec2 *>();

    //approximate destination and start coordinates to the tile
    tdestx = dest->x / TILE_SIZE;
    tdesty = dest->y / TILE_SIZE;
    tstartx = start->x / TILE_SIZE;
    tstarty = start->y / TILE_SIZE;
    destx = ((tdestx - std::floor(tdestx)) >= 0.5f) ? (int) std::floor(tdestx) + 1 : (int) std::floor(tdestx);
    desty = ((tdesty - std::floor(tdesty)) >= 0.5f) ? (int) std::floor(tdesty) + 1 : (int) std::floor(tdesty);
    startx = ((tstartx - std::floor(tstartx)) >= 0.5f) ? (int) std::floor(tstartx) + 1 : (int) std::floor(tstartx);
    starty = ((tstarty - std::floor(tstarty)) >= 0.5f) ? (int) std::floor(tstarty) + 1 : (int) std::floor(tstarty);

    if (room == nullptr) {
        return nullptr;
    }

    //prepare the node map
    if (room != this->room) {
        this->room = room;
        setupNodes(destx, desty);
    } else {
        for (int i = 0; i < ROOM_HEIGHT; i++) {
            for (int j = 0; j < ROOM_WIDTH; j++) {
                if (nodes[i][j] != nullptr) {
                    nodes[i][j]->explored = false;
                    nodes[i][j]->gcost = 10000000000.0f;
                    //these two might be unnecessary but this is just for making 100% sure
                    nodes[i][j]->prev = nullptr;
                    nodes[i][j]->next = nullptr;
                }
            }
        }
    }

    //initialize frontier to starting node
    while (!frontier.empty()) {
        frontier.pop();
    }

    if (nodes[starty][startx] == nullptr || nodes[desty][destx] == nullptr) {
        return nullptr;
    }

    nodes[starty][startx]->gcost = 0;
    frontier.push(nodes[starty][startx]);

    while (!frontier.empty()) {
        currentNode = frontier.top();
        frontier.pop();

        //set here in order to ignore nodes that have been pushed to the frontier multiple times
        //(a node might be reached via multiple paths. First time a node gets popped is via the
        //cheapest path, so it gets ignored every time after that.)
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
            path->push_back(new Vec2{(float) currentNode->x * TILE_SIZE, (float) currentNode->y * TILE_SIZE});

            currentNode = currentNode->next;
        }

        if (path->empty()) {
            return nullptr;
        }

        return path;
    }

    return nullptr;
}

/*
 * Generates the grid of PathNodes for the algorithm to operate with, relative to the currently set room.
 */
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

/*
 * Explores a given node. This means it checks all neighboring nodes to see if they are unexplored and reachable.
 * If they are, they get pushed to the frontier after calculating their gcost and setting up their prev pointer to this node.
 */
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

            //diagonal step logic: diagonal only walkable if unobstructed (both adjacent tiles are not solid)
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
