//
// Created by clode on 21/01/2026.
//

#include "Prop.h"
#include "../globals.h"

Prop::Prop(Game *game, PropType type) : Sprite(game, nullptr, nullptr) {
    switch (type) {
        case TRUCK:
            makeTruck();
            break;
        case BOX:
            makeBox();
            break;
        case BOX_SMALL:
            makeBoxSmall();
            break;
        case TANK:
            makeTank();
            break;
        default:
            makeBox();
            break;
    }
}

void Prop::makeTruck() {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/truck.png",
        32, 64, new Vec2{0, 0},
        frameLocation, 1, 32, 64, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeBox() {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/box.png",
        32, 32, new Vec2{0, 0},
        frameLocation, 1, 32, 32, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeBoxSmall() {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/box_small.png",
        16, 32, new Vec2{0, 0},
        frameLocation, 1, 16, 32, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}

void Prop::makeTank() {
    Vec2 *frameLocation = new Vec2[] {{0, 0}};
    Animation *animation = new Animation(
        "resources/textures/props/tank.png",
        48, 64, new Vec2{0, 0},
        frameLocation, 1, 48, 64, 0, false
    );
    addAnimation("anim", animation);
    playAnimation("anim", 0);
}
