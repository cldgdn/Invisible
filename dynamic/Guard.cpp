#include "Guard.h"

#include <algorithm>
#include <cmath>

#include "Player.h"
#include "../AudioManager.h"
#include "../globals.h"
#include "../Game.h"
#include "../raycasting.h"

using namespace GUARD;

Guard::Guard(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo, std::vector<Vec2 *> *patrolPath) : Sprite(game, fallbackTexture, fallbackUVinfo), hp(3), isAlive(true) {
    this->patrolPath = patrolPath;
    this->isAlerted = false;
    this->isPathNeeded = false;
    this->reversePath = false;
    this->target = nullptr;

    alertMark = new Sprite(game, nullptr, nullptr);
    delete alertMark->transform;
    alertMark->transform = transform;
    addAllAnimations();
    playAnimation("idle_down", 0);
    alertMark->playAnimation("alert", 0);

    Collider *wallCollider = new Collider(
        transform, {1, 1},
        TILE_SIZE - 2, TILE_SIZE - 2,
        0,
        CLAYER_TILES | CLAYER_SOLID_PROPS,
        ColliderType::SOLID, false
    );

    Collider *hurtBox = new Collider(
        transform, {1, -1.0 * TILE_SIZE},
        TILE_SIZE - 2, TILE_SIZE * 2,
        CLAYER_ENEMY,
        CLAYER_PLAYER,
        ColliderType::TRIGGER, false
    );

    colliders["wall"] = wallCollider;
    colliders["hurtBox"] = hurtBox;

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
    Vec2 heading;

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
    }

    if (move){
        heading = getHeadingVersor();
        heading.x *= GUARD::VELOCITY * FIXED_DT;
        heading.y *= GUARD::VELOCITY * FIXED_DT;

        transform->translate2d(heading);
    }
}

void Guard::draw() {
    Sprite::draw();

    if (displayMark > 0.0f) {
        alertMark->draw();
    }
}

void Guard::process() {
    if (!isAlive) return;

    Player *player = game->player;
    Pathfinder *pathfinder = game->pathfinder;
    Room *activeRoom = game->activeRoom;
    Vec2 heading = getHeadingVersor();

    if (displayMark > 0.0f) displayMark -= FIXED_DT;

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

        if (fireTimer > 0) {
            fireTimer -= FIXED_DT;
        } else if (!player->isDead) {
            fire();
            fireTimer = FIRE_COOLDOWN;
        }
    }
    else if (!player->usingBox && !player->isDead) {
        Vec2 origin = transform->position + RAYCAST_OFFSET;

        std::vector<Collider*> v{};
        v.push_back(player->colliders["hurtBox"]);

        RaycastHit playerHit = raycast(&origin, &heading, VIEW_DISTANCE, CLAYER_PLAYER, &v);

        if (playerHit.hit) {
            RaycastHit tileHit = raycast(&origin, &heading, VIEW_DISTANCE, CLAYER_TILES, &activeRoom->tileMap->colliders);

            if (!tileHit.hit || playerHit.distance < tileHit.distance) {
                AudioManager::getInstance().playSound("!", 0.7f, false);
                isAlerted = true;
                displayMark = MARK_DISPLAY_TIMER;
                fireTimer = FIRE_COOLDOWN / 2;
                std::cout << "PLAYER SPOTTED!!" << std::endl;
            }
        }
    }

    if (heading.x == 0 && heading.y == 0) {
        switch (facing) {
            case UP:
                playAnimation("idle_up", 0);
                break;
            case DOWN:
                playAnimation("idle_down", 0);
                break;
            case LEFT:
                playAnimation("idle_left", 0);
                break;
            case RIGHT:
                playAnimation("idle_right", 0);
                break;
            default:
                playAnimation("idle_up", 0);
        }
    } else {
        if (abs(heading.x) >= abs(heading.y)) {
            if (heading.x > 0)
                facing = RIGHT;
            else
                facing = LEFT;
        }
        else {
            if (heading.y > 0)
                facing = DOWN;
            else
                facing = UP;
        }

        switch (facing) {
            case RIGHT:
                if (*currentAnimation != "walk_right")
                    playAnimation("walk_right", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case LEFT:
                if (*currentAnimation != "walk_left")
                    playAnimation("walk_left", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case DOWN:
                if (*currentAnimation != "walk_down")
                    playAnimation("walk_down", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case UP:
                if (*currentAnimation != "walk_up")
                    playAnimation("walk_up", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            default:
                playAnimation("idle_down", 0);
                break;
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

Vec2 Guard::getBulletDirection() {
    Vec2 *pos = &transform->position;
    Vec2 *dest = &game->player->transform->position;

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

void Guard::fire() {
    Bullet *b = new Bullet(game, getBulletDirection());
    switch (facing) {
        case RIGHT:
            b->transform->position = transform->position + BULLET_OFFSET_RIGHT;
            break;
        case LEFT:
            b->transform->position = transform->position + BULLET_OFFSET_LEFT;
            break;
        case DOWN:
            b->transform->position = transform->position + BULLET_OFFSET_DOWN;
            break;
        case UP:
            b->transform->position = transform->position + BULLET_OFFSET_UP;
            break;
        default:
            b->transform->position = transform->position + BULLET_OFFSET_DOWN;
            break;
    }
    game->activeRoom->bullets.push_back(b);

    AudioManager::getInstance().playSound("gun", 0.5f, false);
}

void Guard::takeDamage(int dmg) {
    hp -= dmg;
    if (hp <= 0) {
        die();
    }
}

void Guard::die() {
    isAlive = false;
    isActive = false;
    isVisible = false;
}

void Guard::addAllAnimations() {
    Vec2 *frameLocations = new Vec2[] {
        {0, 1}
    };
    Animation *animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_down", animation);

    frameLocations = new Vec2[] {
        {1, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_up", animation);

    frameLocations = new Vec2[] {
        {0, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_left", animation);

    frameLocations = new Vec2[] {
        {1, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_right", animation);

    frameLocations = new Vec2[] {
        {2, 1},
        {4, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_down", animation);

    frameLocations = new Vec2[] {
        {3, 1},
        {5, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_up", animation);

    frameLocations = new Vec2[] {
        {2, 0},
        {4, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_left", animation);

    frameLocations = new Vec2[] {
        {3, 0},
        {5, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/guard.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_right", animation);

    frameLocations = new Vec2[] {{0,0}};
    animation = new Animation(
        "resources/textures/sprites/alert.png",
        16, 16, new Vec2{8, -1.0 * TILE_SIZE - 8},
        frameLocations, 1, 16,  16, 0, false
    );
    alertMark->addAnimation("alert", animation);
}