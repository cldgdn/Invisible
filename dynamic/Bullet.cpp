#include "Bullet.h"

#include "../globals.h"

using namespace BULLET;

Bullet::Bullet(Game *game, Vec2 direction) : Sprite(game,  new Texture("resources/textures/sprites/bullet.png", 4, 4, Texture::STRETCH, nullptr), nullptr) {
    this->direction = direction;
    Collider *collider = new Collider(
        transform, {1, -1},
        2, 2,
        0,
        CLAYER_TILES | CLAYER_SOLID_PROPS | CLAYER_PLAYER,
        ColliderType::TRIGGER, false
    );
    colliders["hitBox"] = collider;
}

Bullet::~Bullet() {}

void Bullet::process() {
    if (!isActive) return;
    const Vec2 velocity{direction.x * BULLET_SPEED * TILE_SIZE * (float) FIXED_DT, direction.y * BULLET_SPEED * TILE_SIZE * (float) FIXED_DT};
    transform->translate2d(velocity);
}
