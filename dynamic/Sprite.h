#ifndef INVISIBLE_SPRITE_H
#define INVISIBLE_SPRITE_H
#include <vector>

#include "Animation.h"
#include "Collider.h"
#include "Transform.h"

class Game;

enum Facing {
    UP, DOWN, LEFT, RIGHT
};

class Sprite {
public:
    Game *game;
    Transform *transform;
    std::unordered_map<std::string, Collider*> colliders;
    const std::string *currentAnimation;
    Facing facing;
    bool isActive, isVisible;

    Sprite(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo);
    virtual ~Sprite();

    void draw();
    void addAnimation(const std::string& name, Animation *animation);
    bool playAnimation(const std::string& name, int startFrame);
    void stopAnimation();
    void resumeAnimation();

protected:
    std::unordered_map<std::string, Animation*> animations;
    Texture *fallbackTexture;
    UVinfo *fallbackUVinfo;
};


#endif //INVISIBLE_SPRITE_H