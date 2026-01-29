//
// Created by clode on 25/01/2026.
//

#include "Menu.h"

#include "Game.h"

using namespace MENU;

Menu::Menu(Game *game) : game(game) {
    Texture *t = new Texture(
        "resources/textures/logo.png",
        345, 42,
        Texture::STRETCH,
        nullptr
    );
    logo = new Sprite(game, t, nullptr);
    logo->transform->position = {
        (LOGIC_SCREEN_WIDTH - 345) / 2.0f,
        25
    };

    t = new Texture(
        "resources/textures/menu_red.png",
        106, 48,
        Texture::STRETCH,
        nullptr
    );
    UVinfo *uv = new UVinfo{
        {0, 48},
        106, 48
    };
    startGame = new Sprite(game, t, uv);
    startGame->transform->position = {
        (LOGIC_SCREEN_WIDTH - 106) / 2.0f,
        100
    };

    t = new Texture(
        "resources/textures/menu_red.png",
        101, 48,
        Texture::STRETCH,
        nullptr
    );
    uv = new UVinfo{
        {0, 0},
        101, 48
    };
    quitGame = new Sprite(game, t, uv);
    quitGame->transform->position = {
        (LOGIC_SCREEN_WIDTH - 101) / 2.0f,
        160
    };

    t = new Texture(
        "resources/textures/menu_red.png",
        5, 48,
        Texture::STRETCH,
        nullptr
    );
    uv = new UVinfo{
        {131, 48},
        5, 48
    };
    selector = new Sprite(game, t, uv);
}

Menu::~Menu() {
    delete startGame;
    delete quitGame;
    delete logo;
    delete selector;
}

void Menu::processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (onStartGame) {
            if (winScreen) {
                game->stop();
                game->start();
            }
            else {
                game->toggleMenu();
            }
        }
        else {
            game->stop();
        }

        return;
    }

    if (
        glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS
    ) {
        if (!wasKeyPressed)
            onStartGame = !onStartGame;
        wasKeyPressed = true;
    }
    else {
        wasKeyPressed = false;
    }

    if (onStartGame) {
        selector->transform->position.x = startGame->transform->position.x + SELECTOR_OFFSET.x;
        selector->transform->position.y = startGame->transform->position.y + SELECTOR_OFFSET.y;
    }
    else {
        selector->transform->position.x = quitGame->transform->position.x + SELECTOR_OFFSET.x;
        selector->transform->position.y = quitGame->transform->position.y + SELECTOR_OFFSET.y;
    }
}

void Menu::draw() {
    logo->draw();
    startGame->draw();
    quitGame->draw();
    selector->draw();
}
