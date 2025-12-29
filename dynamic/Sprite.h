//
// Created by clode on 19/11/2025.
//

#ifndef INVISIBLE_SPRITE_H
#define INVISIBLE_SPRITE_H
#include <vector>

#include "Animation.h"
#include "Collider.h"
#include "Transform.h"


class Sprite {
public:
    Transform *transform;
    std::vector<Collider*> colliders;
    const std::string *currentAnimation;

    Sprite(Texture *fallbackTexture, UVinfo *fallbackUVinfo);
    ~Sprite();

    void draw();
    void addCollider(Collider *collider);
    void addAnimation(const std::string& name, Animation *animation);
    bool playAnimation(const std::string& name, int startFrame);
    void stopAnimation();
    void resumeAnimation();

private:
    std::unordered_map<std::string, Animation*> animations;
    Texture *fallbackTexture;
    UVinfo *fallbackUVinfo;
};


#endif //INVISIBLE_SPRITE_H