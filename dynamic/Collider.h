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
    Transform *transform;
    Vec2 offset;
    float width, height;
    short unsigned int layersOn, layersCollided;
    ColliderType type;
    bool isActive, isStatic;

    Collider(Transform *transform, Vec2 offset, float width, float height, unsigned int layersOn, unsigned int layersCollided, ColliderType type, bool isStatic);
};

#endif //INVISIBLE_COLLIDER_H