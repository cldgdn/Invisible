//
// Created by clode on 25/01/2026.
//

#ifndef INVISIBLE_MENU_H
#define INVISIBLE_MENU_H
#include "dynamic/Sprite.h"
#include "GLFW/glfw3.h"

namespace MENU {
    constexpr Vec2 SELECTOR_OFFSET = {-6.0f, 0.0f};
}

class Menu {
public:
    Game *game;
    bool winScreen = false;

    Menu(Game *game);
    ~Menu();

    void processInput(GLFWwindow *window);
    void draw();
private:
    Sprite *startGame, *quitGame, *selector, *logo;
    bool onStartGame = true, wasKeyPressed = false;
};


#endif //INVISIBLE_MENU_H