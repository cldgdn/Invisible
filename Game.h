//
// Created by clode on 10/12/2025.
//

#ifndef INVISIBLE_GAME_H
#define INVISIBLE_GAME_H
#include <vector>

#include "Shader.h"
#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "static/Room.h"
#include "static/TileMap.h"


class Game {
public:
    std::vector<Room*> rooms;
    Room *activeRoom;
    GLFWwindow *window;

    Game(GLFWwindow *window);
    ~Game();

    void start();
    void stop();
private:
    bool debug, isRunning;
    Shader *tileShader, *tileDebugShader, *spriteShader, *spriteDebugShader;
};


#endif //INVISIBLE_GAME_H