#ifndef INVISIBLE_GAME_H
#define INVISIBLE_GAME_H
#include <vector>

#include "Menu.h"
#include "Pathfinder.h"
#include "Shader.h"
#include "dynamic/Player.h"
#include "GLFW/glfw3.h"
#include "static/Room.h"
#include "static/TileMap.h"

CollisionType AABB(Collider *a, Collider *b);
Room* makeOutsideRoom(Game *game);
Room* makeEntranceRoom(Game *game);
Room* makeStorageRoom(Game *game);
Room* makeWinRoom(Game *game);


class Game {
public:
    std::unordered_map<std::string, Room*> rooms;
    Room *activeRoom;
    Player *player;
    Pathfinder *pathfinder;
    GLFWwindow *window;
    int score = 0;
    int time = 0;
    bool inStealth;

    Game(GLFWwindow *window);
    ~Game();

    void start();
    void stop(bool saveScore);
    void setRoom(const std::string& name);
    void toggleMenu();
private:
    bool debug, isRunning, isOnMenu, wasKeyPressed = false;
    Shader *tileShader, *spriteShader, *textShader, *boxShader;
    Menu *menu;
    std::unique_ptr<TextObject> text;

    void buildRooms();
    void processInput();
};


#endif //INVISIBLE_GAME_H