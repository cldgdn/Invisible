#ifndef INVISIBLE_GUARD_H
#define INVISIBLE_GUARD_H
#include "Sprite.h"
#include "../globals.h"


namespace GUARD {
    constexpr float VELOCITY = 2.0f * TILE_SIZE; //tiles / second
}

class Guard : public Sprite {
public:
    std::vector<Vec2*> *patrolPath;
    std::vector<Vec2*> *currentPath;
    Vec2 *target;
    int currDest;
    bool isAlerted, isPathNeeded, reversePath;

    Guard(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo, std::vector<Vec2*> *patrolPath);
    ~Guard();

    void moveTowardDest();

private:
    Vec2 getHeadingVersor();
};


#endif //INVISIBLE_GUARD_H