#include "Collider.h"

Collider::Collider(Transform *transform, Vec2 offset, float width, float height, unsigned int layersOn, unsigned int layersCollided, ColliderType type, bool isStatic) :
    transform(transform),
    offset(offset),
    width(width),
    height(height),
    layersOn(layersOn),
    layersCollided(layersCollided),
    type(type),
    isActive(true),
    isStatic(isStatic)
{}