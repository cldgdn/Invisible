//
// Created by clode on 07/11/2025.
//

#include "Collider.h"

Collider::Collider(Transform *transform, Vec2 offset, float width, float height, unsigned int layerMask, ColliderType type, bool isStatic) :
    transform(transform),
    offset(offset),
    width(width),
    height(height),
    layerMask(layerMask),
    type(type),
    isActive(true),
    isStatic(isStatic)
{}