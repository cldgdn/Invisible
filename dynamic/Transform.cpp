//
// Created by clode on 07/11/2025.
//

#include "Transform.h"

Transform::Transform() :
    position(Vec2(0, 0)),
    rotation(Vec2(0, 0)),
    scale(Vec2(1, 1)),
    currentTranslation2d(Vec2(0, 0)),
    translatePending(false)
{}

void Transform::translate2d(const Vec2& translation) {
    currentTranslation2d = translation;

    position.x += translation.x;
    position.y += translation.y;

    translatePending = true;
}

void Transform::confirmTranslate2d() {
    translatePending = false;

    currentTranslation2d.x = 0;
    currentTranslation2d.y = 0;
}

void Transform::undoTranslate2d() {
    if (!translatePending) return;

    position.x -= currentTranslation2d.x;
    position.y -= currentTranslation2d.y;

    currentTranslation2d.x = 0;
    currentTranslation2d.y = 0;
}
