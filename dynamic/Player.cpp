#include "Player.h"

#include "../globals.h"

using namespace PLAYER;

Player::Player(Texture *fallbackTexture, UVinfo *fallbackUVinfo) : Sprite(fallbackTexture, fallbackUVinfo) {
}

Player::~Player() {
}

void Player::processInput(GLFWwindow *window) {
    //MOVEMENT
    Vec2 direction = {0.0f, 0.0f};
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        direction.y -= 1.0f;
    } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        direction.y += 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        direction.x -= 1.0f;
    } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        direction.x += 1.0f;
    }

    if (direction.x != 0.0f || direction.y != 0.0f) {
        direction.x *= VELOCITY * deltaTime;
        direction.y *= VELOCITY * deltaTime;

        transform->translate2d(direction);

        if (currentAnimation == nullptr || *currentAnimation != "walking") {
            playAnimation("walking", 0);
        }
        else if (!(animations[*currentAnimation]->playing)) {
            resumeAnimation();
        }
    }
    else {
        stopAnimation(); //this is where I would put my idle animation... IF I HAD ONE
    }
}
