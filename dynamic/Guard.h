#ifndef INVISIBLE_GUARD_H
#define INVISIBLE_GUARD_H
#include "Sprite.h"


class Guard : public Sprite {
    std::vector<Vec2*> *patrolPath;
    std::vector<Vec2*> *currentPath;
    Vec2 *currDest;
    bool isAlerted, isPathNeeded;
};


#endif //INVISIBLE_GUARD_H