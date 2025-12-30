#include "Game.h"

#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"

static glm::mat4 projection = glm::ortho(
    0.0f, (float) LOGIC_SCREEN_WIDTH,
    (float) LOGIC_SCREEN_HEIGHT, 0.0f,
    -1.0f, 1.0f
);

/*
 * When creating a new Game instance, all shaders, rooms, sprites, tilemaps and everything else is created.
 * This is where objects are hard coded into the game.
 */
Game::Game(GLFWwindow *window) : window(window) {
    spriteShader = new Shader("shaders\\vertex\\spriteVertex.vert", "shaders\\fragment\\visorSupport.frag");
    tileShader = new Shader("shaders\\vertex\\tilesVertex.vert", "shaders\\fragment\\visorSupport.frag");

    rooms = std::vector<Room *>();

    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile1 = new TileMap(map1, "resources\\textures\\tiles\\debug\\");
    auto *room1 = new Room(tile1, nullptr);

    rooms.push_back(room1);

    activeRoom = room1;

    Texture *playerSpriteSheet = new Texture("resources\\spritesheet.png", 80, 16, Texture::TileMode::STRETCH, nullptr);
    Vec2 *frameLocations = new Vec2[]{ //this is not best practice but sunk cost demands its use.
        {0, 0},
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0}
    };
    Animation *playerAnim1 = new Animation("resources\\spritesheet.png", 16, 32, frameLocations, 5, 16, 16, 5);

    player = new Player(playerSpriteSheet, nullptr);
    player->addAnimation("walking", playerAnim1);
}

Game::~Game() {
    delete spriteShader;
    delete tileShader;
    delete spriteDebugShader;
    delete tileDebugShader;

    for (Room *room : rooms) {
        delete room;
    }

    delete player;
}

/*
 * Starting the game instance will begin drawing and processing all the sprites in the current activeRoom.
 */
void Game::start() {
    double currentTime, lastFrame = 0;
    isRunning = true;
    player->transform->position = {40, 0};
    //GAME LOOP
    while (isRunning) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        //TODO: GAME LOGIC

        player->processInput(window);

        //TODO: COLLISION CHECKS
        //run through each non-static collider and check for collisions against all other colliders on the same layers (will be split into sub-areas if it comes out to be too slow)

        if (player->transform->translatePending) {
            for (Collider *c : player->colliders) {
                if (c->layerMask & CLAYER_TILES) {
                    for (Collider *t : activeRoom->tileMap->colliders) {
                        CollisionType type = AABB(c, t);
                        switch (type) {
                            case CollisionType::SOLID_COLLISION:
                                //handle solid by clamping c movement to t boundaries based on direction
                                //printf("[COLLISION] Player collider hit a wall!\n");
                                player->transform->undoTranslate2d();
                                break;
                            case CollisionType::TRIGGER_COLLISION:
                                //printf("[COLLISION] Player collider hit a trigger!\n");
                                break;
                            case CollisionType::NO_COLLISION:
                            default:
                                break;
                        }
                    }
                }
            }

            if (player->transform->translatePending) {
                player->transform->confirmTranslate2d();
            }
        }

        player->transform->confirmTranslate2d(); //<- direct confirmations like this would not happen and would be handled by the colliders themselves based on collision outcome.

        //RENDERING

        glClearColor(1.0f, 1.0, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        tileShader->use();
        glUniformMatrix4fv(glGetUniformLocation(tileShader->ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        tileShader->setInt("uRenderMode", 0);
        tileShader->setInt("uVirtualHeight", LOGIC_SCREEN_HEIGHT);
        tileShader->setInt("uRealHeight", SCREEN_HEIGHT);

        activeRoom->tileMap->draw(false);

        spriteShader->use();
        glUniformMatrix4fv(glGetUniformLocation(spriteShader->ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        spriteShader->setInt("uRenderMode", 0);
        spriteShader->setInt("uVirtualHeight", LOGIC_SCREEN_HEIGHT);
        spriteShader->setInt("uRealHeight", SCREEN_HEIGHT);

        if (activeRoom->guards != nullptr) {
            for (Guard *guard : *activeRoom->guards) {
                guard->draw();
            }
        }

        player->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

        isRunning = !glfwWindowShouldClose(window);
    }
}

/*
 * sets the game's state to not running, which will cause the entire thing to stop running and return to main.
 * this method is intended to be used by game objects (such as sprites ecc.)
 */
void Game::stop() {
    isRunning = false;
}

CollisionType Game::AABB(Collider *a, Collider *b) {
    if (!(a->layerMask & b->layerMask))
        return CollisionType::NO_COLLISION;

    Vec2 apos = a->transform != nullptr ? a->transform->position + a->offset : a->offset;
    Vec2 bpos = b->transform != nullptr ? b->transform->position + b->offset : b->offset;

    bool isRight, isLeft, isAbove, isBelow;
    isRight = apos.x > bpos.x + b->width;
    isLeft = apos.x + a->width < bpos.x;
    isAbove = apos.y + a->height < bpos.y;
    isBelow = apos.y > bpos.y + b->height;

    if (!(isRight || isLeft || isAbove || isBelow)) {
        if (b->type == ColliderType::SOLID)
            return CollisionType::SOLID_COLLISION;

        return CollisionType::TRIGGER_COLLISION;
    }

    return CollisionType::NO_COLLISION;
}
