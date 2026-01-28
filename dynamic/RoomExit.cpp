#include "RoomExit.h"

RoomExit::RoomExit(Game *game, std::string roomName, ExitOrientation orientation) : Sprite(game, nullptr, nullptr), roomName(roomName) {
    Collider *interaction, *solid;
    Texture *t;

    if (orientation == VERTICAL) {
        interaction = new Collider(
            transform, {0, 0},
            TILE_SIZE, 2 * TILE_SIZE,
            CLAYER_INTERACTION, 0,
            ColliderType::TRIGGER, true
        );
        solid = new Collider(
            transform, {3, 0},
            TILE_SIZE - 6, TILE_SIZE * 32,
            CLAYER_SOLID_PROPS, 0,
            ColliderType::SOLID, true
        );

        t = new Texture(
            "resources/textures/sprites/door_vertical.png",
            16, 32,
            Texture::STRETCH, nullptr
        );
    }
    else {
        interaction = new Collider(
            transform, {0, 0},
            2 * TILE_SIZE, TILE_SIZE,
            CLAYER_INTERACTION, 0,
            ColliderType::TRIGGER, true
        );
        solid = new Collider(
            transform, {0, 3},
            TILE_SIZE * 32, TILE_SIZE - 6,
            CLAYER_SOLID_PROPS, 0,
            ColliderType::SOLID, true
        );

        t = new Texture(
            "resources/textures/sprites/door_horizontal.png",
            32, 16,
            Texture::STRETCH, nullptr
        );
    }

    fallbackTexture = t;
    colliders["exit"] = interaction;
    colliders["solid"] = solid;
}
