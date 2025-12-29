//
// Created by clode on 18/11/2025.
//

#include "Animation.h"

#include "../globals.h"
#include "GLFW/glfw3.h"

Animation::Animation(const std::string& atlas, float width, float height, Vec2 *frameLocations, int frameCount, int frameWidth, int frameHeight, int FPS) :
    atlas(atlas), width(width), height(height), frameLocations(frameLocations), frameCount(frameCount), frameWidth(frameWidth), frameHeight(frameHeight), FPS(FPS)
{
    texture = new Texture(atlas, width, height, Texture::STRETCH, nullptr);
}

Animation::~Animation() {
    delete texture;
    free(frameLocations);
}

void Animation::bindTransform(Transform *transform) {
    this->transform = transform;
}

void Animation::play(int startingFrame) {
    timer = 0;
    currentFrame = startingFrame;
    playing = true;
}

void Animation::stop() {
    playing = false;
}

void Animation::resume() {
    play(currentFrame);
}

void Animation::draw() {
    if (playing) {
        double TPF = 1.0f / FPS;
        if (timer >= TPF) {
            timer = 0;
            currentFrame++;
            if (currentFrame >= frameCount) currentFrame = 0;
        }

        timer += deltaTime;
    }

    UVinfo info = {
        {
            frameLocations[currentFrame].x * frameWidth,
            frameLocations[currentFrame].y * frameHeight,
        },
        width = frameWidth,
        height = frameHeight
    };

    Vec2 pos;
    if (transform == nullptr)
        pos = {0, 0};
    else
        pos = transform->position;

    texture->draw(pos, &info, false);
}
