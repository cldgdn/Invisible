#ifndef INVISIBLE_RAYCASTING_H
#define INVISIBLE_RAYCASTING_H
#include <vector>

#include "data/structs.h"
#include "dynamic/Collider.h"

struct RaycastHit {
    bool hit = false;
    float distance = 0.0f;
    Vec2 point{0, 0};
    Collider *collider = nullptr;
};

inline Vec2 colliderMin(Collider *c) {
    Vec2 origin{
        (c->transform != nullptr ? c->transform->position.x : 0.0f) + c->offset.x,
        (c->transform != nullptr ? c->transform->position.y : 0) + c->offset.y
    };
    return origin;
}

inline Vec2 colliderMax(Collider *c) {
    Vec2 origin = colliderMin(c);
    return {
        origin.x + c->width,
        origin.y + c->height
    };
}



RaycastHit raycastAABB(Vec2 *origin, Vec2 *direction, Collider *collider);
RaycastHit raycast(Vec2 *origin, Vec2 *direction, float maxDistance, short unsigned int layerMask, std::vector<Collider*> *colliders);

#endif //INVISIBLE_RAYCASTING_H