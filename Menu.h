//
// Created by clode on 25/01/2026.
//

#ifndef INVISIBLE_MENU_H
#define INVISIBLE_MENU_H
#include "globals.h"
#include "dynamic/Sprite.h"
#include "GLFW/glfw3.h"
#include "text/TextObject.h"

namespace MENU {
    constexpr int OPTIONS_AMT = 3;
    constexpr int OPTIONS_OFFSET_FROM_BOTTOM = TILE_SIZE;
    constexpr int OPTIONS_SPACE = 4;
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
    std::unique_ptr<TextObject> title;
    std::unique_ptr<TextObject> selectorL, selectorR;
    std::unique_ptr<TextObject> options[MENU::OPTIONS_AMT];
    int index = 0;
    bool wasKeyPressed = false, displayLeaderboards = false;
};


#endif //INVISIBLE_MENU_H