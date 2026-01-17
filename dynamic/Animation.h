//
// Created by clode on 18/11/2025.
//

#ifndef INVISIBLE_ANIMATOR_H
#define INVISIBLE_ANIMATOR_H
#include <functional>
#include <string>

#include "Transform.h"
#include "../static/Texture.h"


class Animation {
public:
    int currentFrame, frameCount, FPS;
    int frameWidth, frameHeight;
    float width, height;
    bool playing, loop;
    std::function<void()> callback;

    Animation(const std::string& atlas, float width, float height, Vec2 *offset, Vec2 *frameLocations, int frameCount, int frameWidth, int frameHeight, int FPS, bool loop);
    ~Animation();

    void bindTransform(Transform *transform);
    void play(int startingFrame);
    void stop();
    void resume();

    void draw();

private:
    double timer, frameProgress;

    Transform *transform;
    std::string atlas;
    Vec2 *frameLocations;
    Vec2 *offset;

    Texture *texture;
};


#endif //INVISIBLE_ANIMATOR_H