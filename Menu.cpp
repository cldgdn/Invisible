//
// Created by clode on 25/01/2026.
//

#include "Menu.h"

#include "Game.h"
#include "text/TextManager.h"

using namespace MENU;

Menu::Menu(Game *game) : game(game) {
    TextManager& tm = TextManager::getInstance();

    title = tm.createText("Speedtest", 64, "invisible");
    title->scale = 1.7f;
    title->position = {
        (LOGIC_SCREEN_WIDTH - title->getWidth() * title->scale) / 2,
        24
    };
    title->color = glm::vec4(0.9, 0.0, 0.0, 0.4);

    selectorL = tm.createText("Helvetica", 128, "!");
    selectorR = tm.createText("Helvetica", 128, "!");

    selectorL->scale = 0.5f;
    selectorR->scale = 0.5f;

    options[0] = tm.createText("Helvetica", 128, "START GAME");
    options[1] = tm.createText("Helvetica", 128, "EXIT GAME");
    options[2] = tm.createText("Helvetica", 128, "LEADERBOARDS");

    for (int i = 0; i < OPTIONS_AMT; i++) {
        options[i]->scale = 0.5f;
    }

    float currOffset = OPTIONS_OFFSET_FROM_BOTTOM;
    for (int i = OPTIONS_AMT - 1; i > -1; i--) {
        options[i]->position = {
            (LOGIC_SCREEN_WIDTH - options[i]->getWidth() * options[i]->scale) / 2,
            (LOGIC_SCREEN_HEIGHT - currOffset) - options[i]->getHeight() * options[i]->scale
        };
        currOffset += options[i]->getHeight() * options[i]->scale + OPTIONS_SPACE;
    }

    index = 0;
    selectorL->position.x = options[index]->position.x - OPTIONS_SPACE - selectorL->getWidth() * options[index]->scale;
    selectorL->position.y = options[index]->position.y;

    selectorR->position.x = options[index]->position.x + options[index]->getWidth() * options[index]->scale + OPTIONS_SPACE;
    selectorR->position.y = options[index]->position.y;
}

Menu::~Menu() {
}

void Menu::processInput(GLFWwindow *window) {
    bool upPressed = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    bool downPressed = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);

    if (!wasKeyPressed) {
        if (upPressed) {
            index = (index - 1);
            if (index < 0) index = OPTIONS_AMT -1;
            wasKeyPressed = true;
        } else if (downPressed) {
            index = (index + 1);
            if (index > OPTIONS_AMT - 1) index = 0;
            wasKeyPressed = true;
        }
    } else if ((!downPressed) && (!upPressed)) {
        wasKeyPressed = false;
    }

    if (upPressed || downPressed) {
        selectorL->position.x = options[index]->position.x - OPTIONS_SPACE - selectorL->getWidth() * options[index]->scale;
        selectorL->position.y = options[index]->position.y;

        selectorR->position.x = options[index]->position.x + options[index]->getWidth() * options[index]->scale + OPTIONS_SPACE;
        selectorR->position.y = options[index]->position.y;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        switch (index) {
            case 0:
                if (winScreen) {
                    game->stop();
                    game->start();
                    break;
                }
                game->toggleMenu();
                break;
            case 1:
                game->stop();
                break;
            default:
                std::cerr << "[Menu::OOB] Literally how." << std::endl;
        }
    }
}

void Menu::draw() {
    title->draw();
    selectorL->draw();
    selectorR->draw();

    for (int i = 0; i < OPTIONS_AMT; i++) {
        if (options[i] != nullptr)
            options[i]->draw();
    }
}
