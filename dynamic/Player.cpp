#include "Player.h"

#include <algorithm>

#include "../AudioManager.h"
#include "../Game.h"
#include "../globals.h"

using namespace PLAYER;

Player::Player(Game *game, Texture *fallbackTexture, UVinfo *fallbackUVinfo) : Sprite(game, fallbackTexture, fallbackUVinfo), usingBox(false) {
    //VISUALS
    addAllAnimations();
    playAnimation("idle_down", 0);

    //COLLIDERS
    Collider *wallCollider = new Collider(
        transform, {2, 5},
        12, 8,
        0,
        CLAYER_TILES | CLAYER_SOLID_PROPS,
        ColliderType::SOLID, false
    );

    Collider *hurtBox = new Collider(
        transform, {2, (-1.0f * TILE_SIZE) + 2},
        TILE_SIZE - 4, TILE_SIZE * 2 - 4,
        CLAYER_PLAYER,
        CLAYER_INTERACTION | CLAYER_ENEMY,
        ColliderType::TRIGGER, false
    );

    Collider *punchBox = new Collider(
        transform, {-2, (-1.0f * TILE_SIZE) - 2},
        TILE_SIZE + 4, TILE_SIZE + 4,
        0,
        CLAYER_ENEMY | CLAYER_INTERACTION,
        ColliderType::TRIGGER, false
    );
    punchBox->isActive = false;

    colliders["wall"] = wallCollider;
    colliders["hurtBox"] = hurtBox;
    colliders["punchBox"] = punchBox;
}

Player::~Player() {
}

void Player::processInput(GLFWwindow *window) {
    if (isDead) return;

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

    //ACTIONS
    usingBox = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ) {
        if (!wasNVGKeyPressed) {
            wasNVGKeyPressed = true;
            usingNVG = !usingNVG;
            if (usingNVG)
                AudioManager::getInstance().playSound("nvg", 1.0, false);
        }
    }
    else {
        wasNVGKeyPressed = false;
    }

    bool punchPressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
    bool punchJustPressed = !wasPunchPressed && punchPressed;
    wasPunchPressed = punchPressed;

    if (punchJustPressed && !usingBox && isPunching <= 0.0f) {
        isPunching = PUNCH_DURATION;
    }

    colliders["punchBox"]->isActive = isPunching > 0.0f;

    if (isPunching > 0.0f) {
        isPunching -= FIXED_DT;

        if (isPunching <= 0.0f) {
            guardsHit.clear();
        }

        switch (facing) {
            case UP:
                playAnimation("punch_up", 0);
                break;
            case DOWN:
                playAnimation("punch_down", 0);
                break;
            case LEFT:
                playAnimation("punch_left", 0);
                break;
            case RIGHT:
                playAnimation("punch_right", 0);
                break;
            default:
                playAnimation("punch_down", 0);
                break;
        }
    }
    else if (usingBox) {
        playAnimation("box", 0);
    }
    else if (direction.x != 0.0f || direction.y != 0.0f) {
        if (abs(direction.x) >= abs(direction.y)) {
            if (direction.x > 0.0f)
                facing = RIGHT;
            else
                facing = LEFT;
        }
        else {
            if (direction.y > 0.0f)
                facing = DOWN;
            else
                facing = UP;
        }

        direction.x *= VELOCITY * FIXED_DT;
        direction.y *= VELOCITY * FIXED_DT;

        transform->translate2d(direction);

        switch (facing) {
            case RIGHT:
                if (*currentAnimation != "walk_right")
                    playAnimation("walk_right", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case LEFT:
                if (*currentAnimation != "walk_left")
                    playAnimation("walk_left", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case DOWN:
                if (*currentAnimation != "walk_down")
                    playAnimation("walk_down", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            case UP:
                if (*currentAnimation != "walk_up")
                    playAnimation("walk_up", 0);
                else if (!animations[*currentAnimation]->playing)
                    resumeAnimation();
                break;
            default:
                playAnimation("idle_down", 0);
                break;
        }
    }
    else {
        switch (facing) {
            case RIGHT:
                playAnimation("idle_right", 0);
                break;
            case LEFT:
                playAnimation("idle_left", 0);
                break;
            case DOWN:
                playAnimation("idle_down", 0);
                break;
            case UP:
                playAnimation("idle_up", 0);
                break;
            default:
                playAnimation("idle_down", 0);
                break;
        }
    }
}

void Player::die() {
    if (isDead) return;

    AudioManager::getInstance().playSound("death_scream", 1.0f, false);
    playAnimation("death", 0);
    isDead = true;
}


void Player::addAllAnimations() {
    Vec2 *frameLocations = new Vec2[]{ //this is not best practice but sunk cost demands its use.
        {0, 1}
    };
    Animation *animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_down", animation);

    frameLocations = new Vec2[] {
        {1, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_up", animation);

    frameLocations = new Vec2[] {
        {0, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_left", animation);

    frameLocations = new Vec2[] {
        {1, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("idle_right", animation);

    frameLocations = new Vec2[] {
        {2, 1},
        {4, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_down", animation);

    frameLocations = new Vec2[] {
        {3, 1},
        {5, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_up", animation);

    frameLocations = new Vec2[] {
        {2, 0},
        {4, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_left", animation);

    frameLocations = new Vec2[] {
        {3, 0},
        {5, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 2, 16, 32, 4, true
    );
    addAnimation("walk_right", animation);

    frameLocations = new Vec2[] {
        {8, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE + 6},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("box", animation);

    frameLocations = new Vec2[] {
        {6, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("punch_down", animation);

    frameLocations = new Vec2[] {
        {7, 1}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("punch_up", animation);

    frameLocations = new Vec2[] {
        {6, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("punch_left", animation);

    frameLocations = new Vec2[] {
        {7, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 1, 16, 32, 0, false
    );
    addAnimation("punch_right", animation);

    frameLocations = new Vec2[] {
        {8, 0},
        {8, 0},
        {9, 0},
        {9, 0}
    };
    animation = new Animation(
        "resources/textures/sprites/player.png",
        16, 32, new Vec2{0, -1.0 * TILE_SIZE},
        frameLocations, 4, 16, 32, 1.14f, false);
    animation->callback = [this]() {
        this->game->stop();
        this->game->start();
    };
    addAnimation("death", animation);
}
