//
// Created by clode on 25/01/2026.
//

#include "Menu.h"

#include "Game.h"
#include "ScoreManager.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "text/TextManager.h"

using namespace MENU;

Menu::Menu(Game *game) : game(game), box("resources/models/scatola/scatola.obj") {
    TextManager &tm = TextManager::getInstance();

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

    selectorR->position.x = options[index]->position.x + options[index]->getWidth() * options[index]->scale +
                            OPTIONS_SPACE;
    selectorR->position.y = options[index]->position.y;
}

Menu::~Menu() {
}

void Menu::processInput(GLFWwindow *window) {
    bool upPressed =
        (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        || (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        || (scrollY > 0);
    bool downPressed =
        (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        || (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        || (scrollY < 0);
    bool acceptPressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    if (scrollY != 0)
        scrollY = 0;
    if (scrollX != 0)
        scrollX = 0;

    if (!wasKeyPressed) {
        if (upPressed) {
            index = (index - 1);
            if (index < 0) index = OPTIONS_AMT -1;
            wasKeyPressed = true;
        } else if (downPressed) {
            index = (index + 1);
            if (index > OPTIONS_AMT - 1) index = 0;
            wasKeyPressed = true;
        } else if (acceptPressed) {
            wasKeyPressed = true;

            if (displayLeaderboards) {
                displayLeaderboards = false;
            } else {
                switch (index) {
                    case 0:
                        if (startScreen) {
                            game->setRoom("outside");
                            startScreen = false;
                        }
                        else if (winScreen) {
                            game->stop(true);
                            game->start();
                            break;
                        }
                        game->toggleMenu();
                        break;
                    case 1:
                        game->stop(false);
                        break;
                    case 2:
                        displayLeaderboards = true;
                        break;
                    default:
                        std::cerr << "[Menu::OOB] Literally how." << std::endl;
                }
            }
        }
    } else if ((!downPressed) && (!upPressed) && (!acceptPressed)) {
        wasKeyPressed = false;
    }

    if (upPressed || downPressed) {
        selectorL->position.x = options[index]->position.x - OPTIONS_SPACE - selectorL->getWidth() * options[index]->scale;
        selectorL->position.y = options[index]->position.y;

        selectorR->position.x = options[index]->position.x + options[index]->getWidth() * options[index]->scale + OPTIONS_SPACE;
        selectorR->position.y = options[index]->position.y;
    }
}

void Menu::draw(Shader *textShader, Shader *boxShader) {
    if (!displayLeaderboards) {
        if (startScreen) {
            boxAngle += BOX_TURN_ANGLE * deltaTime;

            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);

            boxShader->use();
            glm::mat4 projection = glm::perspective(
                glm::radians(45.0f),
                (GLfloat)LOGIC_SCREEN_WIDTH / (GLfloat)LOGIC_SCREEN_HEIGHT,
                0.1f,
                100.0f
            );

            glm::mat4 view = glm::mat4(1.0f);
            glUniform3f(glGetUniformLocation(boxShader->ID, "viewPos"), 0, 0, 0);
            glUniform3f(glGetUniformLocation(boxShader->ID, "lightPos"), 5, 5, 5);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0, 0, -5));
            model = glm::scale(model, glm::vec3(0.75f));
            model = glm::rotate(model, glm::radians(boxAngle), glm::vec3(0, 1, 0));

            glUniformMatrix4fv(glGetUniformLocation(boxShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(boxShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(glGetUniformLocation(boxShader->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

            box.draw(*boxShader);

            glDisable(GL_DEPTH_TEST);
        }

        textShader->use();

        title->draw();
        selectorL->draw();
        selectorR->draw();

        for (int i = 0; i < OPTIONS_AMT; i++) {
            if (options[i] != nullptr)
                options[i]->draw();
        }
    } else {
        ScoreManager::getInstance().draw();
    }
}
