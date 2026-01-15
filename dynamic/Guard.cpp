#include "Guard.h"

#include <algorithm>
#include <cmath>

#include "Player.h"
#include "../globals.h"
#include "../Game.h"
#include "../raycasting.h"

using namespace GUARD;

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
    float minMotion = GUARD::VELOCITY * FIXED_DT;

    if (currentPath->size() > 1 && abs(pos->x - dest->x) < minMotion && abs(pos->y - dest->y) < minMotion) {
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

void Guard::process() {
    Player *player = game->player;
    Pathfinder *pathfinder = game->pathfinder;
    Room *activeRoom = game->activeRoom;

    if (isAlerted) {
        isPathNeeded = true;

        if (target == nullptr) {
            target = new Vec2{player->transform->position.x, player->transform->position.y};
        }
        else if (abs(target->x - player->transform->position.x) > TILE_SIZE || abs(target->y - player->transform->position.y) > TILE_SIZE) {
            target->x = player->transform->position.x;
            target->y = player->transform->position.y;
        }
        else {
            isPathNeeded = false;
        }

        if (isPathNeeded) {
            if (currentPath != nullptr && currentPath != patrolPath) {
                for (Vec2 *v : *currentPath) {
                    delete v;
                }
                currentPath->clear();
                delete currentPath;
            }

            currentPath = pathfinder->findPath(activeRoom, &transform->position, target);
            reversePath = false;
            currDest = 0;
        }
    }
    else if (!player->usingBox) {
        Vec2 heading = getHeadingVersor();
        Vec2 origin = transform->position + RAYCAST_OFFSET;

        RaycastHit playerHit = raycast(&origin, &heading, VIEW_DISTANCE, CLAYER_PLAYER, &player->colliders);

        if (playerHit.hit) {
            RaycastHit tileHit = raycast(&origin, &heading, VIEW_DISTANCE, CLAYER_TILES, &activeRoom->tileMap->colliders);

            if (!tileHit.hit || playerHit.distance < tileHit.distance) {
                isAlerted = true;
                std::cout << "PLAYER SPOTTED!!" << std::endl;
            }
        }
    }
    moveTowardDest();
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
