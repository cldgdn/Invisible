//
// Created by clode on 07/11/2025.
//

#ifndef INVISIBLE_COLLIDER_H
#define INVISIBLE_COLLIDER_H

#include "../data/structs.h"
#include "Transform.h"

enum ColliderType {
    SOLID,
    TRIGGER
};

enum CollisionType {
    NO_COLLISION,
    SOLID_COLLISION,
    TRIGGER_COLLISION
};

class Collider {
public:
    const Transform *transform;
    Vec2 offset;
    float width, height;
    unsigned int layerMask;
    ColliderType type;
    bool active;

    Collider(const Transform *transform, Vec2 offset, float width, float height, unsigned int layerMask, ColliderType type);

    CollisionType collide();
};

#endif //INVISIBLE_COLLIDER_H