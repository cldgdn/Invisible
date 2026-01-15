#ifndef INVISIBLE_GAME_H
#define INVISIBLE_GAME_H
#include <vector>

#include "Pathfinder.h"
#include "Shader.h"
#include "dynamic/Player.h"
#include "GLFW/glfw3.h"
#include "static/Room.h"
#include "static/TileMap.h"

CollisionType AABB(Collider *a, Collider *b);


class Game {
public:
    std::vector<Room*> rooms;
    Room *activeRoom;
    Player *player;
    Pathfinder *pathfinder;
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