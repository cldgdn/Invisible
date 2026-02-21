#include "BoxPickup.h"

#include "../globals.h"

BoxPickup::BoxPickup(Game *game) : Sprite(game, nullptr, nullptr) {
    fallbackTexture = new Texture(
        "resources/textures/sprites/player.png",
        16, 32,
        Texture::STRETCH, nullptr
    );
    fallbackUVinfo = new UVinfo(
        Vec2(8 * 16, 32),
        16, 32
    );

    Collider *c = new Collider(
        transform, {0, 0},
        16, 20,
        CLAYER_INTERACTION,
        0,
        ColliderType::TRIGGER, true
    );

    colliders["box"] = c;
}

BoxPickup::~BoxPickup() {
}
