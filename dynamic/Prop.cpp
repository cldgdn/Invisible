//
// Created by clode on 21/01/2026.
//

#include "Prop.h"
#include "../globals.h"

Prop::Prop(Game *game, PropType type, const std::string& postfix) : Sprite(game, nullptr, nullptr) {
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
