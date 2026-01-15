#include "raycasting.h"
#include <algorithm>
#include <limits>
#include <vector>

RaycastHit raycastAABB(Vec2 *origin, Vec2 *direction, Collider *collider) {
    Vec2 min = colliderMin(collider);
    Vec2 max = colliderMax(collider);

    float tMin = -std::numeric_limits<float>::infinity();
    float tMax = std::numeric_limits<float>::infinity();

    if (direction->x != 0.0f) {
        float tx1 = (min.x - origin->x) / direction->x;
        float tx2 = (max.x - origin->x) / direction->x;

        tMin = std::max(tMin, std::min(tx1, tx2));
        tMax = std::min(tMax, std::max(tx1, tx2));
    }
    else if (origin->x < min.x || origin->x > max.x) {
        return {};
    }

    if (direction->y != 0.0f) {
        float ty1 = (min.y - origin->y) / direction->y;
        float ty2 = (max.y - origin->y) / direction->y;

        tMin = std::max(tMin, std::min(ty1, ty2));
        tMax = std::min(tMax, std::max(ty1, ty2));
    }
    else if (origin->y < min.y || origin->y > max.y) {
        return {};
    }

    if (tMax < tMin || tMax < 0.0f) {
        return {};
    }

    float t = (tMin >= 0.0f) ? tMin : tMax;

    return {
        true,
        t,
        {origin->x + direction->x * t, origin->y + direction->y * t},
        collider
    };
}

RaycastHit raycast(Vec2 *origin, Vec2 *direction, float maxDistance, short unsigned int layerMask, std::vector<Collider*> *colliders) {
    RaycastHit closest;
    closest.distance = maxDistance;

    for (Collider *collider : *colliders) {
        if (collider == nullptr || !collider->isActive) continue;
        if (!(collider->layersOn & layerMask)) continue;

        RaycastHit hit = raycastAABB(origin, direction, collider);

        if (hit.hit && hit.distance < closest.distance) {
            closest = hit;
        }
    }

    return closest;
}