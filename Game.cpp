#include "Game.h"

#include <cmath>

#include "Pathfinder.h"
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
    spriteShader = new Shader("shaders/vertex/spriteVertex.vert", "shaders/fragment/visorSupport.frag");
    tileShader = new Shader("shaders/vertex/tilesVertex.vert", "shaders/fragment/visorSupport.frag");
    spriteDebugShader = nullptr;
    tileDebugShader = nullptr;

    pathfinder = new Pathfinder();

    rooms = std::vector<Room *>();

    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile1 = new TileMap(map1, "resources/textures/tiles/debug/");
    auto *room1 = new Room(tile1, nullptr);
    room1->guards = new std::vector<Guard *>();

    Texture *guardTemp = new Texture(
            "resources/textures/tiles/debug/wall/pixil-frame-0.png",
            16, 16,
            Texture::TileMode::STRETCH,
            nullptr
    );

    std::vector<Vec2*> *guard0Patrol = new std::vector<Vec2*>();
    guard0Patrol->push_back(new Vec2(13 * TILE_SIZE, 1 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(10 * TILE_SIZE, 5 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(10 * TILE_SIZE, 6 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(8 * TILE_SIZE, 6 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(8 * TILE_SIZE, 5 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(8 * TILE_SIZE, 6 * TILE_SIZE));
    guard0Patrol->push_back(new Vec2(3 * TILE_SIZE, 6 * TILE_SIZE));

    Guard *guard0 = new Guard(this, guardTemp, nullptr, guard0Patrol);
    guard0->transform->position.x = 13 * TILE_SIZE;
    guard0->transform->position.y = 1 * TILE_SIZE;

    room1->guards->push_back(guard0);

    rooms.push_back(room1);

    activeRoom = room1;

    Texture *playerSpriteSheet = new Texture("resources/textures/sprites/player.png", 160, 64, Texture::TileMode::STRETCH, nullptr);
    UVinfo *playerUVinfo = new UVinfo(
        {0.0f, 0.0f},
        160, 64
    );
    player = new Player(this, playerSpriteSheet, playerUVinfo);
}

Game::~Game() {
    delete spriteShader;
    delete tileShader;
    delete spriteDebugShader;
    delete tileDebugShader;
    delete pathfinder;

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
    double elapsedTime = 0, frameStartTime, frameEndTime, totFrameTime = 0, physTimeAccumulator = 0;
    int frames = 0;
    long totframes = 0;
    isRunning = true;
    player->transform->position = {40, 0};

    //GAME LOOP
    while (isRunning) {
        frameStartTime = glfwGetTime();

        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        //Prevent death spiral
        if (deltaTime >= 0.25) {
            deltaTime = 0.25;
        }

        physTimeAccumulator += deltaTime;

        //TODO: GAME LOGIC

        while (physTimeAccumulator >= FIXED_DT) {
            for (Guard *guard : *(activeRoom->guards)) {
                guard->process();
            }

            player->processInput(window);

            //COLLISION CHECKS
            //run through each non-static collider and check for collisions against all other colliders on the same layers (will be split into sub-areas if it comes out to be too slow)

            if (player->transform->translatePending) {
                Collider *c = player->colliders["wall"];
                if (c->layersCollided & CLAYER_TILES) {
                    for (Collider *t : activeRoom->tileMap->colliders) {
                        AABB(c, t);
                    }
                }

                if (player->transform->translatePending) {
                    player->transform->confirmTranslate2d();
                }
            }

            for (Guard *guard : *activeRoom->guards) {
                if (guard->transform->translatePending) {
                    Collider *c = guard->colliders["wall"];
                    if (c->layersCollided & CLAYER_TILES) {
                        for (Collider *t : activeRoom->tileMap->colliders) {
                            AABB(c, t);
                        }
                    }

                    if (guard->transform->translatePending) {
                        guard->transform->confirmTranslate2d();
                    }
                }
            }

            physTimeAccumulator -= FIXED_DT;
        }



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

        frameEndTime = glfwGetTime();

        totFrameTime += frameEndTime - frameStartTime;
        frames++;
        totframes++;
        elapsedTime += deltaTime;

        if (elapsedTime >= 1) {
            elapsedTime = 0;
            //printf("FPS: %d -- AVG frametime: %f\n", frames, totFrameTime / frames);
            totFrameTime = 0;
            frames = 0;
        }

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

/*
 * checks if a collides with b and what kind of collision it is. For solid collisions, it assumes that a is the collider moving INTO the solid object.
 * This assumption is viable since moving solid objects will only be checking against walls and other static objects by design.
 */
CollisionType AABB(Collider *a, Collider *b) {
    if (!(a->layersCollided & b->layersOn))
        return CollisionType::NO_COLLISION;

    Vec2 apos = a->transform != nullptr ? a->transform->position + a->offset : a->offset;
    Vec2 bpos = b->transform != nullptr ? b->transform->position + b->offset : b->offset;

    bool isRight, isLeft, isAbove, isBelow;
    isRight = apos.x >= bpos.x + b->width;
    isLeft = apos.x + a->width <= bpos.x;
    isAbove = apos.y + a->height <= bpos.y;
    isBelow = apos.y >= bpos.y + b->height;

    if (!(isRight || isLeft || isAbove || isBelow)) {
        if (b->type == ColliderType::SOLID) {
            // handle sliding collision (if a is moving toward b at an angle, only push a back along the axis it actually came at it from)
            float deltaxright, deltaxleft, deltax, deltaydown, deltayup, deltay;
            if (a->transform != nullptr && !a->isStatic) { //<- makes sure the object can in fact be moved
                //find out how far into the b collider a got along the x-axis, assuming it couldn't have crossed more than halfway through it in 1 frame.
                deltax = 0;
                if (a->transform->currentTranslation2d.x != 0) {
                    deltaxright = apos.x + a->width - bpos.x;
                    deltaxleft = bpos.x + b->width - apos.x;

                    if (deltaxright < deltaxleft)
                        deltax = deltaxright;
                    else
                        deltax = deltaxleft;
                }

                //find out how far into the b collider a got along the y-axis, assuming it couldn't have crossed more than halfway through it in 1 frame.
                deltay = 0;
                if (a->transform->currentTranslation2d.y != 0) {
                    deltaydown = apos.y + a->height - bpos.y;
                    deltayup = bpos.y + b->height - apos.y;

                    if (deltaydown < deltayup)
                        deltay = deltaydown;
                    else
                        deltay = deltayup;
                }

                //if either delta is 0, skip comparing them.
                if (deltay == 0) goto movex;
                if (deltax == 0) goto movey;

                //clamp a to b based on which movement would be smaller
                if (deltax < deltay) {
                    movex:

                    if (a->transform->currentTranslation2d.x > 0) {
                        a->transform->currentTranslation2d.x -= deltax;
                        a->transform->position.x -= deltax;
                    }
                    else if (a->transform->currentTranslation2d.x < 0) {
                        a->transform->currentTranslation2d.x += deltax;
                        a->transform->position.x += deltax;
                    }
                }
                else {
                    movey:

                    if (a->transform->currentTranslation2d.y > 0) {
                        a->transform->currentTranslation2d.y -= deltay;
                        a->transform->position.y -= deltay;
                    }
                    else if (a->transform->currentTranslation2d.y < 0) {
                        a->transform->currentTranslation2d.y += deltay;
                        a->transform->position.y += deltay;
                    }
                }
            }
            return CollisionType::SOLID_COLLISION;
        }

        return CollisionType::TRIGGER_COLLISION;
    }

    return CollisionType::NO_COLLISION;
}

