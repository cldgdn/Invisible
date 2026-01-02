#ifndef INVISIBLE_PATHFINDER_H
#define INVISIBLE_PATHFINDER_H
#include <queue>

#include "static/Room.h"
#include "static/TileMap.h"

struct PathNode {
    float hcost;
    float gcost;
    bool explored;
    PathNode *prev;
};

struct PathNodeCompare {
    bool operator()(const PathNode* a, const PathNode* b) const {
        return (a->gcost + a->hcost) > (b->gcost + b->hcost);
        // "true" means a has LOWER priority than b
    }
};

class Pathfinder {
public:
    Room *room;
    PathNode* nodes[ROOM_HEIGHT][ROOM_WIDTH];
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeCompare> frontier;

    Pathfinder();
    ~Pathfinder();

    std::vector<Vec2*> findPath(Room *room, Vec2 start, Vec2 dest);

private:
    void setupNodes();
    float heuristic(int nodex, int nodey, int destx, int desty);
    void explore(int nodex, int nodey);
};


#endif //INVISIBLE_PATHFINDER_H