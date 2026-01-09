#ifndef INVISIBLE_GUARD_H
#define INVISIBLE_GUARD_H
#include "Sprite.h"


class Guard : public Sprite {
public:
    std::vector<Vec2*> *patrolPath;
    std::vector<Vec2*> *currentPath;
    Vec2 *currDest;
    bool isAlerted, isPathNeeded;

    Guard(Texture *fallbackTexture, UVinfo *fallbackUVinfo, std::vector<Vec2*> *patrolPath);
    ~Guard();

    bool moveTowardDest();

private:
};


#endif //INVISIBLE_GUARD_H