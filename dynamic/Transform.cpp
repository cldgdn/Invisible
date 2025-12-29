//
// Created by clode on 07/11/2025.
//

#include "Transform.h"

Transform::Transform() :
    position(Vec2(0, 0)),
    rotation(Vec2(0, 0)),
    scale(Vec2(1, 1)),
    currentTranslation2d(Vec2(0, 0))
{}

void Transform::translate2d(const Vec2& translation) {
    currentTranslation2d = translation;

    position.x += translation.x;
    position.y += translation.y;

    // TODO: Call on the colliders to check for solid collision or undo types collisions.
    // if solid clamp coordinates to wall, if undo call undoTranslate2d.
}

void Transform::undoTranslate2d() {
    position.x -= currentTranslation2d.x;
    position.y -= currentTranslation2d.y;

    currentTranslation2d.x = 0;
    currentTranslation2d.y = 0;
}
