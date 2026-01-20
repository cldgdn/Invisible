//
// Created by clode on 18/01/2026.
//

#ifndef INVISIBLE_BULLET_H
#define INVISIBLE_BULLET_H
#include "Sprite.h"

namespace BULLET {
    constexpr float BULLET_SPEED = 4.0f; //tiles / second
}

class Bullet : public Sprite {
public:
    Vec2 direction;

    Bullet(Game *game, Vec2 direction);
    ~Bullet();

    void process();
};


#endif //INVISIBLE_BULLET_H