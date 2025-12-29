//
// Created by clode on 10/12/2025.
//

#ifndef INVISIBLE_GAME_H
#define INVISIBLE_GAME_H
#include "Shader.h"


class Game {
public:
    Game();
    ~Game();

    void start();
    void stop();
private:
    bool debug, isRunning;
    Shader *tileShader, *tileDebugShader, *spriteShader, *spriteDebugShader;
};


#endif //INVISIBLE_GAME_H