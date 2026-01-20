#ifndef INVISIBLE_GUARD_H
#define INVISIBLE_GUARD_H
#include "Sprite.h"
#include "../globals.h"


namespace GUARD {
    constexpr float VELOCITY = 2.0f * TILE_SIZE; //tiles / second
    constexpr Vec2 RAYCAST_OFFSET = Vec2(TILE_SIZE * 0.5f, TILE_SIZE * 0.5f);
    constexpr float VIEW_DISTANCE = TILE_SIZE * 10.0f;
    constexpr double MARK_DISPLAY_TIMER = 1.0f;
    constexpr double FIRE_COOLDOWN = 0.8f;
    constexpr Vec2 BULLET_OFFSET_DOWN = Vec2(5, -3);
    constexpr Vec2 BULLET_OFFSET_UP = Vec2(5, -1.0 *TILE_SIZE);
    constexpr Vec2 BULLET_OFFSET_RIGHT = Vec2(TILE_SIZE, -2);
    constexpr Vec2 BULLET_OFFSET_LEFT = Vec2(0, -2);
}

class Guard : public Sprite {
public:
    std::vector<Vec2*> *patrolPath;
    std::vector<Vec2*> *currentPath;
    Sprite *alertMark;
    Vec2 *target;
    int currDest, hp;
    bool isAlive, isAlerted, isPathNeeded, reversePath;

    Guard(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo, std::vector<Vec2*> *patrolPath);
    ~Guard();

    void moveTowardDest();

    void draw();

    void process();
    void takeDamage(int dmg);
    void die();
    void fire();

private:
    double displayMark = 0, fireTimer = 0;

    Vec2 getHeadingVersor();
    Vec2 getBulletDirection();
    void addAllAnimations();
};


#endif //INVISIBLE_GUARD_H