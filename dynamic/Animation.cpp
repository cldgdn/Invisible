#include "Animation.h"

#include "../globals.h"
#include "GLFW/glfw3.h"

Animation::Animation(const std::string& atlas, float width, float height, Vec2 *offset, Vec2 *frameLocations, int frameCount, int frameWidth, int frameHeight, int FPS, bool loop) :
    atlas(atlas), width(width), height(height), offset(offset), frameLocations(frameLocations), frameCount(frameCount), frameWidth(frameWidth), frameHeight(frameHeight), FPS(FPS), loop(loop)
{
    timer = 0;
    frameProgress = 0;
    texture = new Texture(atlas, width, height, Texture::STRETCH, nullptr);
    callback = nullptr;
}

Animation::~Animation() {
    delete texture;
    delete offset;
    free(frameLocations);
}

void Animation::bindTransform(Transform *transform) {
    this->transform = transform;
}

void Animation::play(int startingFrame) {
    if (frameProgress != 0)
        timer = frameProgress;
    else {
        timer = 0;
        frameProgress = 0;
    }
    currentFrame = startingFrame;
    playing = true;
}

void Animation::stop() {
    frameProgress = timer;
    playing = false;
}

void Animation::resume() {
    play(currentFrame);
}

void Animation::draw() {
    if (playing && FPS != 0) {
        double TPF = 1.0f / FPS;
        if (timer >= TPF) {
            while (timer >= TPF) {
                currentFrame++;
                timer -= TPF;
            }
            if (currentFrame >= frameCount) {
                if (loop)
                    currentFrame -= frameCount;
                else if (callback != nullptr) {
                    callback();
                    stop();
                    currentFrame = frameCount - 1;
                }
            }
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

    Vec2 *pos;
    if (transform == nullptr)
        pos = offset;
    else
        pos = &transform->position;

    texture->draw(*pos + *offset, &info, false);
}
