#include "Game.h"

#include <cmath>

#include "AudioManager.h"
#include "Pathfinder.h"
#include "ScoreManager.h"
#include "dynamic/RoomExit.h"
#include "GLFW/glfw3.h"
#include "glm/gtc/type_ptr.hpp"
#include "text/TextManager.h"

static glm::mat4 projection = glm::ortho(
    0.0f, (float) LOGIC_SCREEN_WIDTH,
    (float) LOGIC_SCREEN_HEIGHT, 0.0f,
    -1.0f, 1.0f
);

/*
 * When creating a new Game instance, everything that is consistent across the entire game is loaded.
 * This means: shaders are compiled, audio is loaded and the Player and Menu are created.
 */
Game::Game(GLFWwindow *window) : window(window), isOnMenu(true) {
    spriteShader = new Shader("shaders/vertex/spriteVertex.vert", "shaders/fragment/visorSupport.frag");
    tileShader = new Shader("shaders/vertex/tilesVertex.vert", "shaders/fragment/visorSupport.frag");
    textShader = new Shader("shaders/vertex/text.vert", "shaders/fragment/text.frag");

    AudioManager* am = &AudioManager::getInstance();
    am->loadSound("gun", "resources/sounds/gun.wav");
    am->loadSound("death_scream", "resources/sounds/death_scream.wav");
    am->loadSound("!", "resources/sounds/!.wav");
    am->loadSound("nvg", "resources/sounds/nvg.wav");

    TextManager* tm = &TextManager::getInstance();
    tm->loadFont("resources/fonts/Speedtest-2O7nK.ttf", "Speedtest", 64);
    tm->loadFont("resources/fonts/Helvetica Ultra Compressed.otf", "Helvetica", 128);

    pathfinder = new Pathfinder();

    Texture *playerSpriteSheet = new Texture("resources/textures/sprites/player.png", 160, 64, Texture::TileMode::STRETCH, nullptr);
    UVinfo *playerUVinfo = new UVinfo(
        {0.0f, 0.0f},
        160, 64
    );
    player = new Player(this, playerSpriteSheet, playerUVinfo);

    menu = new Menu(this);
}

Game::~Game() {
    delete spriteShader;
    delete tileShader;
    delete textShader;
    delete pathfinder;
    delete menu;

    for (auto [name, room] : rooms) {
        if (room != nullptr)
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
    std::vector<Guard*> delayedGuards;
    inStealth = true;
    isRunning = true;
    isOnMenu = true;
    menu->winScreen = false;
    player->isDead = false;
    player->playAnimation("idle_down", 0);

    score = 0;
    time = 0;

    buildRooms();
    setRoom("outside");

    //GAME LOOP
    while (isRunning) {
        frameStartTime = glfwGetTime();

        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        time += deltaTime;

        //Prevent death spiral
        if (deltaTime >= 0.25) {
            deltaTime = 0.25;
        }

        physTimeAccumulator += (isOnMenu) ? 0 : deltaTime;

        //GAME LOGIC

        if (isOnMenu) {
            menu->processInput(window);
        }
        else {
            processInput();
        }

        while (physTimeAccumulator >= FIXED_DT && !isOnMenu) {
            for (Guard *guard : (activeRoom->guards)) {
                if (!guard->isActive) continue;
                guard->process();
            }

            for (Bullet *bullet : (activeRoom->bullets)) {
                bullet->process();
            }

            player->processInput(window);

            //COLLISION CHECKS
            //run through each non-static collider and check for collisions against all other colliders on the same layers (will be split into sub-areas if it comes out to be too slow)

            if (player->isPunching > 0.0f) {
                Collider *c = player->colliders["punchBox"];
                for (Guard *guard : (activeRoom->guards)) {
                    if (!guard->isActive || !guard->isAlive) continue;

                    bool washit = false;

                    for (Guard *hit : player->guardsHit) {
                        if (hit == guard) {
                            washit = true;
                        }
                    }

                    if (washit) continue;

                    Collider *g = guard->colliders["hurtBox"];

                    if (AABB(c, g) == CollisionType::TRIGGER_COLLISION) {
                        player->guardsHit.push_back(guard);
                        if (guard->isAlerted)
                            guard->takeDamage(PLAYER::PUNCH_DAMAGE);
                        else
                            guard->die();
                    }
                }

                if (activeRoom->exit != nullptr) {
                    Collider *e = activeRoom->exit->colliders["exit"];
                    if (AABB(c, e) != NO_COLLISION) {
                        setRoom(activeRoom->exit->roomName);
                    }
                }
            }

            if (player->transform->translatePending) {
                Collider *c = player->colliders["wall"];
                if (c->layersCollided & CLAYER_TILES) {
                    for (Collider *t : activeRoom->tileMap->colliders) {
                        AABB(c, t);
                    }
                }

                if (activeRoom->exit != nullptr) {
                    Collider *e = activeRoom->exit->colliders["solid"];
                    AABB(c, e);
                }

                if (player->transform->translatePending) {
                    player->transform->confirmTranslate2d();
                }
            }

            for (Guard *guard : activeRoom->guards) {
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

            for (Bullet *bullet : activeRoom->bullets) {
                if (!bullet->isActive) continue;

                if (bullet->transform->translatePending) {
                    Collider *c = bullet->colliders["hitBox"];
                    for (auto &[name, t] : player->colliders) {
                        if (AABB(c, t) == CollisionType::TRIGGER_COLLISION) {
                            player->die();
                            bullet->isActive = false;
                            break;
                        }
                    }

                    if (!bullet->isActive) continue;

                    for (Collider *t : activeRoom->tileMap->colliders) {
                        if (AABB(c, t) == CollisionType::SOLID_COLLISION) {
                            bullet->isActive = false;
                            break;
                        }
                    }
                }

                for (auto b = activeRoom->bullets.begin(); b != activeRoom->bullets.end();) {
                    if (!(*b)->isActive) {
                        delete *b;
                        b = activeRoom->bullets.erase(b);
                    } else {
                        ++b;
                    }
                }
            }

            physTimeAccumulator -= FIXED_DT;
        }

        //RENDERING

        //if something in logic decided to stop the game (exit from pause), rooms may have been deleted, causing segfault if attempted to draw
        if (!isRunning) break;

        int renderMode = 0;
        if (player->usingNVG) {
            renderMode = (activeRoom->isDark) ? 1 : 3;
        } else if (activeRoom->isDark) {
            renderMode = 2;
        }

        glClearColor(1.0f, 1.0, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        tileShader->use();
        glUniformMatrix4fv(glGetUniformLocation(tileShader->ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        tileShader->setInt("uRenderMode", renderMode);
        tileShader->setInt("uVirtualHeight", LOGIC_SCREEN_HEIGHT);
        tileShader->setInt("uRealHeight", SCREEN_HEIGHT);

        activeRoom->tileMap->draw(false);

        spriteShader->use();
        glUniformMatrix4fv(glGetUniformLocation(spriteShader->ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        spriteShader->setInt("uRenderMode", renderMode);
        spriteShader->setInt("uVirtualHeight", LOGIC_SCREEN_HEIGHT);
        spriteShader->setInt("uRealHeight", SCREEN_HEIGHT);

        for (Prop *prop : activeRoom->props) {
            prop->draw();
        }

        if (activeRoom->exit != nullptr)
            activeRoom->exit->draw();

        //guards that are lower on the screen should be drawn in front of the player
        for (Guard *guard : activeRoom->guards) {
            if (guard->transform->position.y < player->transform->position.y)
                guard->draw();
            else
                delayedGuards.push_back(guard);
        }

        player->draw();

        for (Guard *guard : delayedGuards) {
            guard->draw();
        }
        delayedGuards.clear();

        for (Bullet *b : activeRoom->bullets) {
            b->draw();
        }

        if (isOnMenu) {
            textShader->use();
            glUniformMatrix4fv(glGetUniformLocation(textShader->ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
            textShader->setInt("uText", 0);
            spriteShader->setInt("uRenderMode", 0);
            menu->draw();
        }

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

            if (inStealth && !isOnMenu) {
                score += SCORING::MAINTAIN_STEALTH;
            }
            if (!player->isDead && !isOnMenu) {
                score += SCORING::STAY_ALIVE;
            }
        }

        isRunning = !glfwWindowShouldClose(window) && isRunning;
    }
}

/*
 * sets the game's state to not running, which will cause the entire thing to stop running and return to main.
 * this method is intended to be used by game objects (such as sprites ecc.)
 */
void Game::stop(bool saveScore) {
    isRunning = false;
    if (player->usingNVG) player->usingNVG = false;
    for (auto [name, room] : rooms) {
        delete room;
        rooms[name] = nullptr;
    }

    if (saveScore) {
        ScoreManager::getInstance().saveScore({score, time, 0});
    }
}

/*
 * checks if a collides with b and what kind of collision it is. For solid collisions, it assumes that a is the collider moving INTO the solid object b.
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

void Game::setRoom(const std::string &name) {
    inStealth = true;
    score += SCORING::KILL;
    if (name == "win") {
        score += SCORING::GAME_BEAT_BONUS;
        isOnMenu = true;
        menu->winScreen = true;
        if (player->usingNVG) player->usingNVG = false;
    }

    activeRoom = rooms[name];
    player->transform->position.x = activeRoom->playerStartPos.x * TILE_SIZE;
    player->transform->position.y = activeRoom->playerStartPos.y * TILE_SIZE;
}

void Game::toggleMenu() {
    isOnMenu = !isOnMenu;
    if (!isOnMenu)
        player->isPunching = 0;
}

void Game::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        toggleMenu();
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!wasKeyPressed) {
            setRoom("outside");
            wasKeyPressed = true;
        }
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!wasKeyPressed) {
            setRoom("entrance");
            wasKeyPressed = true;

        }
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!wasKeyPressed) {
            setRoom("storage");
            wasKeyPressed = true;
        }
    } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!wasKeyPressed) {
            setRoom("win");
            wasKeyPressed = true;
        }
    } else {
        wasKeyPressed = false;
    }
}

void Game::buildRooms() {
    rooms["outside"] = makeOutsideRoom(this);
    rooms["entrance"] = makeEntranceRoom(this);
    rooms["storage"] = makeStorageRoom(this);
    rooms["win"] = makeWinRoom(this);
}

Room* makeOutsideRoom(Game *game)  {
    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile = new TileMap(map1, "resources/textures/tiles/outside/", true);
    auto *room = new Room({1, 2}, tile, false);

    room->exit = new RoomExit(game, "entrance", VERTICAL);
    room->exit->transform->position = {31 * TILE_SIZE, 8 * TILE_SIZE};

    Prop *p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {4 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {8 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {12 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {16 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {4 * TILE_SIZE, 9 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {8 * TILE_SIZE, 9 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {12 * TILE_SIZE, 9 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TRUCK, "_outside");
    p->transform->position = {16 * TILE_SIZE, 9 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TANK, "_outside");
    p->transform->position = {21 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, TANK, "_outside");
    p->transform->position = {21 * TILE_SIZE, 9 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {8 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {12 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {11 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {2 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    Guard *g = new Guard(game);
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 3 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 3 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 11 * TILE_SIZE});
    g->transform->position = {10 * TILE_SIZE, 3 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 6 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{3 * TILE_SIZE, 6 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{3 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 1 * TILE_SIZE});
    g->transform->position = {18 * TILE_SIZE, 6 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{3 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{3 * TILE_SIZE, 13 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 13 * TILE_SIZE});
    g->transform->position = {18 * TILE_SIZE, 8 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 1 * TILE_SIZE});
    g->transform->position = {29 * TILE_SIZE, 17 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{28 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{28 * TILE_SIZE, 16 * TILE_SIZE});
    g->transform->position = {28 * TILE_SIZE, 0 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{20 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{20 * TILE_SIZE, 14 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 14 * TILE_SIZE});
    g->transform->position = {18 * TILE_SIZE, 16 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    return room;
}

Room* makeEntranceRoom(Game *game) {
    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
        {1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1},
        {1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile = new TileMap(map1, "resources/textures/tiles/inside/", true);
    auto *room = new Room({0, 9}, tile, false);

    room->exit = new RoomExit(game, "storage", VERTICAL);
    room->exit->transform->position = {31 * TILE_SIZE, 8 * TILE_SIZE};

    Prop *p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {3 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {4 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {3 * TILE_SIZE, 6 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {3 * TILE_SIZE, 10 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {10 * TILE_SIZE, 12 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {10 * TILE_SIZE, 4 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {13 * TILE_SIZE, 4 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL, "");
    p->transform->position = {14 * TILE_SIZE, 4 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX, "");
    p->transform->position = {5 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX, "");
    p->transform->position = {4 * TILE_SIZE, 10 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX, "");
    p->transform->position = {2 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX, "");
    p->transform->position = {13 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {13 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {19 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {19 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {23 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {23 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {27 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, DESK, "_inside");
    p->transform->position = {27 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    Guard *g = new Guard(game);
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 8 * TILE_SIZE});
    g->transform->position = {2 * TILE_SIZE, 8 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 9 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 9 * TILE_SIZE});
    g->transform->position = {2 * TILE_SIZE, 9 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 3 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{9 * TILE_SIZE, 2 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{12 * TILE_SIZE, 2 * TILE_SIZE});
    g->transform->position = {11 * TILE_SIZE, 8 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{12 * TILE_SIZE, 9 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{12 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{15 * TILE_SIZE, 12 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{15 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{12 * TILE_SIZE, 16 * TILE_SIZE});
    g->transform->position = {12 * TILE_SIZE, 9 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{11 * TILE_SIZE, 15 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{5 * TILE_SIZE, 15 * TILE_SIZE});
    g->transform->position = {11 * TILE_SIZE, 11 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 7 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 1 * TILE_SIZE});
    g->transform->position = {18 * TILE_SIZE, 7 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 7 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 7 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{27 * TILE_SIZE, 14 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 14 * TILE_SIZE});
    g->transform->position = {22 * TILE_SIZE, 7 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{30 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{30 * TILE_SIZE, 6 * TILE_SIZE});
    g->transform->position = {30 * TILE_SIZE, 11 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 10 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 10 * TILE_SIZE});
    g->transform->position = {18 * TILE_SIZE, 10 * TILE_SIZE};
    g->loopPath = false;
    room->guards.push_back(g);

    return room;
}

Room* makeStorageRoom(Game *game) {
    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1},
        {1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1},
        {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile = new TileMap(map1, "resources/textures/tiles/inside/", true);
    auto *room = new Room({0, 9}, tile, true);

    room->exit = new RoomExit(game, "win", HORIZONTAL);
    room->exit->transform->position = {28 * TILE_SIZE, 0};

    Prop *p = new Prop(game, BOX);
    p->transform->position = {2 * TILE_SIZE, 6 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {4 * TILE_SIZE, 10 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {3 * TILE_SIZE, 14 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {11 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {11 * TILE_SIZE, 4 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {12 * TILE_SIZE, 12 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {10 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {19 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {21 * TILE_SIZE, 6 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {20 * TILE_SIZE, 12 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {20 * TILE_SIZE, 14 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {22 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {25 * TILE_SIZE, 1 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {25 * TILE_SIZE, 3 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {26 * TILE_SIZE, 7 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {27 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX);
    p->transform->position = {29 * TILE_SIZE, 6 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {1 * TILE_SIZE, 3 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {3 * TILE_SIZE, 10 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {4 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {5 * TILE_SIZE, 3 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {5 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {11 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {12 * TILE_SIZE, 14 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {13 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {13 * TILE_SIZE, 15 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {19 * TILE_SIZE, 6 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {19 * TILE_SIZE, 11 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {19 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {21 * TILE_SIZE, 4 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {22 * TILE_SIZE, 13 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {23 * TILE_SIZE, 3 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {24 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {27 * TILE_SIZE, 2 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {27 * TILE_SIZE, 5 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {29 * TILE_SIZE, 12 * TILE_SIZE};
    room->props.push_back(p);

    p = new Prop(game, BOX_SMALL);
    p->transform->position = {29 * TILE_SIZE, 14 * TILE_SIZE};
    room->props.push_back(p);

    Guard *g = new Guard(game);
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{13 * TILE_SIZE, 8 * TILE_SIZE});
    g->transform->position = {2 * TILE_SIZE, 8 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 9 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{13 * TILE_SIZE, 9 * TILE_SIZE});
    g->transform->position = {2 * TILE_SIZE, 9 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 8 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{14 * TILE_SIZE, 8 * TILE_SIZE});
    g->transform->position = {25 * TILE_SIZE, 8 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 9 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{14 * TILE_SIZE, 9 * TILE_SIZE});
    g->transform->position = {25 * TILE_SIZE, 9 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{6 * TILE_SIZE, 6 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{6 * TILE_SIZE, 2 * TILE_SIZE});
    g->transform->position = {6 * TILE_SIZE, 6 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{6 * TILE_SIZE, 12 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 12 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{2 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{6 * TILE_SIZE, 16 * TILE_SIZE});
    g->transform->position = {6 * TILE_SIZE, 12 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{14 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{14 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{10 * TILE_SIZE, 1 * TILE_SIZE});
    g->transform->position = {14 * TILE_SIZE, 1 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{15 * TILE_SIZE, 2 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{17 * TILE_SIZE, 2 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{17 * TILE_SIZE, 7 * TILE_SIZE});
    g->transform->position = {15 * TILE_SIZE, 2 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{15 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{15 * TILE_SIZE, 14 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 14 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{18 * TILE_SIZE, 10 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 10 * TILE_SIZE});
    g->transform->position = {15 * TILE_SIZE, 11 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{19 * TILE_SIZE, 16 * TILE_SIZE});
    g->transform->position = {22 * TILE_SIZE, 16 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{23 * TILE_SIZE, 7 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{23 * TILE_SIZE, 5 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 5 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{22 * TILE_SIZE, 3 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{19 * TILE_SIZE, 3 * TILE_SIZE});
    g->transform->position = {23 * TILE_SIZE, 7 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 10 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{25 * TILE_SIZE, 7 * TILE_SIZE});
    g->transform->position = {25 * TILE_SIZE, 10 * TILE_SIZE};
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{26 * TILE_SIZE, 11 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{26 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{30 * TILE_SIZE, 16 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{30 * TILE_SIZE, 11 * TILE_SIZE});
    g->transform->position = {26 * TILE_SIZE, 11 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    g = new Guard(game);
    g->patrolPath->push_back(new Vec2{28 * TILE_SIZE, 1 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{28 * TILE_SIZE, 5 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 5 * TILE_SIZE});
    g->patrolPath->push_back(new Vec2{29 * TILE_SIZE, 1 * TILE_SIZE});
    g->transform->position = {28 * TILE_SIZE, 1 * TILE_SIZE};
    g->loopPath = true;
    room->guards.push_back(g);

    return room;
}

Room* makeWinRoom(Game *game) {
    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile = new TileMap(map1, "resources/textures/tiles/outside/", true);
    auto *room = new Room({0, 9 * TILE_SIZE}, tile, false);

    room->playerStartPos = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};

    return room;
}