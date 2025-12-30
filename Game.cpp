//
// Created by clode on 10/12/2025.
//

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

    rooms = std::vector<Room*>();

    bool solidMap1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    bool *map1[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        map1[i] = solidMap1[i];
    }
    auto *tile1 = new TileMap(map1, "resources\\textures\\tiles\\debug\\");
    auto *room1 = new Room(tile1, nullptr);

    rooms.push_back(room1);

    activeRoom = room1;
}

Game::~Game() {
    delete spriteShader;
    delete tileShader;
    delete spriteDebugShader;
    delete tileDebugShader;

    for (Room *room : rooms) {
        delete room;
    }
}

/*
 * Starting the game instance will begin drawing and processing all the sprites in the current activeRoom.
 */
void Game::start() {
    Texture spriteSheet("resources\\spritesheet.png", 80, 16, Texture::TileMode::STRETCH, nullptr);
    Sprite numbers(&spriteSheet, nullptr);
    Vec2 frameLocations[] = {
        {0, 0},
        {1, 0},
        {2, 0},
        {3, 0},
        {4, 0}
    };
    Animation anim("resources\\spritesheet.png", 16, 16, frameLocations, 5, 16, 16, 5);
    numbers.addAnimation("numbers",  &anim);

    double currentTime, lastFrame = 0;
    isRunning = true;

    //GAME LOOP
    while (isRunning) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        //TODO: GAME LOGIC

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

        numbers.draw();

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
