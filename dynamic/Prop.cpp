#include "Prop.h"

#include "../globals.h"

Prop::Prop(Game *game, PropType type, const std::string& postfix) : Sprite(game, nullptr, nullptr), type(type) {
    switch (type) {
        case TRUCK:
            makeTruck(postfix);
            break;
        case BOX:
            makeBox(postfix);
            break;
        case BOX_SMALL:
            makeBoxSmall(postfix);
            break;
        case TANK:
            makeTank(postfix);
            break;
        case DESK:
            makeDesk(postfix);
            break;
        case BACKGROUND:
            makeBackground(postfix);
            break;
        default:
            makeBox(postfix);
            break;
    }
}

void Prop::makeTruck(const std::string& postfix) {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/truck" + postfix + ".png",
        32, 64, new Vec2{0, 0},
        frameLocation, 1, 32, 64, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeBox(const std::string& postfix) {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/box" + postfix + ".png",
        32, 32, new Vec2{0, 0},
        frameLocation, 1, 32, 32, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeBoxSmall(const std::string& postfix) {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/box_small" + postfix + ".png",
        16, 32, new Vec2{0, 0},
        frameLocation, 1, 16, 32, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeTank(const std::string& postfix) {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/tank" + postfix + ".png",
        48, 64, new Vec2{0, 0},
        frameLocation, 1, 48, 64, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeDesk(const std::string &postfix) {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/desk" + postfix + ".png",
        32, 32, new Vec2{0, 0},
        frameLocation, 1, 32, 32, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeBackground(const std::string &name) {
    fallbackTexture = new Texture(
        "resources/textures/props/background_" + name + ".png",
        LOGIC_SCREEN_WIDTH,     LOGIC_SCREEN_HEIGHT,
        Texture::TILE, nullptr
    );
}
