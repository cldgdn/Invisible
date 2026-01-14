#include "Guard.h"

#include <algorithm>
#include <cmath>

#include "../globals.h"

Guard::Guard(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo, std::vector<Vec2 *> *patrolPath) : Sprite(game, fallbackTexture, fallbackUVinfo) {
    this->patrolPath = patrolPath;
    this->isAlerted = false;
    this->isPathNeeded = false;
    this->reversePath = false;
    this->target = nullptr;

    Collider *wallCollider = new Collider(
        transform, {1, 1},
        TILE_SIZE - 2, TILE_SIZE - 2,
        0,
        CLAYER_TILES | CLAYER_SOLID_PROPS,
        ColliderType::SOLID, false
    );

    Collider *hurtBox = new Collider(
        transform, {0, -1.0 * TILE_SIZE},
        TILE_SIZE, TILE_SIZE * 2,
        CLAYER_ENEMY,
        CLAYER_PLAYER,
        ColliderType::TRIGGER, false
    );

    colliders.push_back(wallCollider);
    colliders.push_back(hurtBox);

    currentPath = patrolPath;
    currDest = 0;
}

Guard::~Guard() {
    if (currentPath != patrolPath) {
        for (Vec2 *v : *currentPath) {
            delete v;
        }
        currentPath->clear();
        delete currentPath;
    }

    for (Vec2 *v : *patrolPath) {
        delete v;
    }
    patrolPath->clear();
    delete patrolPath;
    delete target;
}

void Guard::moveTowardDest() {
    if (currentPath == nullptr)
        currentPath = patrolPath;

    Vec2 *pos = &transform->position;
    Vec2 *dest = (*currentPath)[currDest];

    bool move = true;

    if (currentPath->size() > 1 && abs(pos->x - dest->x) < 0.05 && abs(pos->y - dest->y) < 0.05) {
    //if (floor(pos->x) == floor(dest->x) && floor(pos->y) == floor(dest->y)) {
        if (isAlerted && currDest == currentPath->size() - 1) {
            move = false;
        }
        else {
            currDest = currDest + (reversePath ? -1 : 1);

            if (currDest == 0 || currDest == currentPath->size() - 1) {
                reversePath = !reversePath;
            }
        }
        //printf("Guard going toward (%f, %f)\n", (*currentPath)[currDest]->x / TILE_SIZE, (*currentPath)[currDest]->y / TILE_SIZE);
    }

    if (move){
        Vec2 heading = getHeadingVersor();
        heading.x *= GUARD::VELOCITY * FIXED_DT;
        heading.y *= GUARD::VELOCITY * FIXED_DT;

        transform->translate2d(heading);
    }
}

Vec2 Guard::getHeadingVersor() {
    if (currentPath == nullptr || currDest >= currentPath->size()) {
        return {0.0f, 0.0f};
    }

    Vec2 *pos = &transform->position;
    Vec2 *dest = (*currentPath)[currDest];

    Vec2 heading {
        dest->x - pos->x,
        dest->y - pos->y
    };

    float length = std::sqrt(heading.x * heading.x + heading.y * heading.y);

    if (length == 0.0f) {
        return {0.0f, 0.0};
    }

    return {
        heading.x / length,
        heading.y / length
    };
}
