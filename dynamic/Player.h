#ifndef INVISIBLE_PLAYER_H
#define INVISIBLE_PLAYER_H
#include "Sprite.h"
#include "../globals.h"
#include "GLFW/glfw3.h"

class Guard;

namespace PLAYER {
    constexpr float VELOCITY = 2.0f * TILE_SIZE; // tiles / second
    constexpr double PUNCH_DURATION = 0.15f; //how long the player stays in a punch animation
    constexpr int PUNCH_DAMAGE = 1;
}

class Player : public Sprite {
public:
    bool usingBox, isDead = false;
    double isPunching;
    std::vector<Guard*> guardsHit; //guards that have been hit by the current punch (so that they don't die in 3 frames from the same lingering hitbox

    Player(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo);
    ~Player();

    void processInput(GLFWwindow *window);
    void die();
private:
    bool wasPunchPressed = false;
    void addAllAnimations();
};


#endif //INVISIBLE_PLAYER_H