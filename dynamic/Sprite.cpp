//
// Created by clode on 19/11/2025.
//

#include "Sprite.h"
#include "../Game.h"

Sprite::Sprite(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo) :
    game(game), fallbackTexture(fallbackTexture), fallbackUVinfo(fallbackUVinfo), currentAnimation(nullptr), isActive(true), isVisible(true)
{
    transform = new Transform();
}

Sprite::~Sprite() {
    delete transform;
    delete fallbackTexture;

    for (auto &[name, collider] : colliders) { delete collider; }
    for (auto &[name, animation] : animations) { delete animation; }
    colliders.clear();
    animations.clear();
}

void Sprite::draw() {
    if (!isActive || !isVisible) return;

    if (currentAnimation != nullptr) {
        auto it = animations.find(*currentAnimation);
        if (it != animations.end()) {
            it->second->draw();
        }
    } else if (fallbackTexture != nullptr) {
        fallbackTexture->draw(transform->position, fallbackUVinfo, false);
    }
}

void Sprite::addAnimation(const std::string &name, Animation *animation) {
    animation->bindTransform(transform);
    animations[name] = animation;
}

bool Sprite::playAnimation(const std::string &name, int startFrame) {
    if (currentAnimation != nullptr) {
        auto it = animations.find(*currentAnimation);
        if (it != animations.end()) {
            it->second->stop();
        }
    }

    auto it = animations.find(name);

    if (it != animations.end()) {
        currentAnimation = &it->first;
        it->second->play(startFrame);
        return true;
    }

    return false;
}

void Sprite::stopAnimation() {
    if (currentAnimation == nullptr) return;

    auto it = animations.find(*currentAnimation);

    if (it != animations.end()) {
        it->second->stop();
    }
}

void Sprite::resumeAnimation() {
    auto it = animations.find(*currentAnimation);

    if (it != animations.end()) {
        it->second->resume();
    }
}
