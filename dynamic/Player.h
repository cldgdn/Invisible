#ifndef INVISIBLE_PLAYER_H
#define INVISIBLE_PLAYER_H
#include "Sprite.h"
#include "../globals.h"
#include "GLFW/glfw3.h"

namespace PLAYER {
    constexpr float VELOCITY = 2.0f * TILE_SIZE; // tiles / second
}

class Player : public Sprite {
public:

    Player(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo);
    ~Player();

    void processInput(GLFWwindow *window);
};


#endif //INVISIBLE_PLAYER_H