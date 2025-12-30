#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "Shader.h"
#include "Shape.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Game.h"
#include "glm/gtc/type_ptr.hpp"
#include "static/Texture.h"
#include "static/TileMap.h"
#include "globals.h"
#include "dynamic/Sprite.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, Sprite* sprite);
bool checkForLoadBearingSnakes();

std::string loadShaderSource(const char* filePath);
GLuint compileShader(const char* filePath, GLenum shaderType);

// settings
unsigned int SCREEN_WIDTH = 1600;
unsigned int SCREEN_HEIGHT = 900;

unsigned int LOGIC_SCREEN_WIDTH = 32 * TILE_SIZE;
unsigned int LOGIC_SCREEN_HEIGHT = 18 * TILE_SIZE;

double deltaTime = 0;
int renderMode = 0;

int main()
{
    if (!checkForLoadBearingSnakes()) return -12;
    std::cout << "Snakes found!" << std::endl;

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "No one hears a word", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool fullTileDisplayMap[ROOM_HEIGHT][ROOM_WIDTH] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,1,0,0,1,0,0,0,1,0,0,0,1,1,1,0},
        {0,0,0,1,1,1,1,0,1,1,0,0,1,0,1,0},
        {0,0,0,0,0,1,0,1,1,0,0,0,1,1,1,0},
        {0,1,1,1,0,0,0,0,1,0,1,0,0,0,0,0},
        {0,1,1,1,0,1,1,1,0,0,1,0,0,1,1,0},
        {0,1,1,1,0,0,0,0,0,0,1,0,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0}
    };

    bool test_map1[ROOM_HEIGHT][ROOM_WIDTH] = {
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
        {0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,1},
        {0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
        {0,1,1,0,1,1,1,1,0,1,0,0,1,1,0,0},
        {0,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0},
        {0,0,1,1,0,0,0,0,0,0,0,0,1,1,0,0},
        {0,1,1,1,0,0,0,1,1,1,0,0,1,0,0,0},
        {0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,1}
    };

    bool basicRoom[ROOM_HEIGHT][ROOM_WIDTH] = {
        {1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,0},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,1,1,1,0,0,1,0,0,0,0,0,1},
        {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    bool (*map)[16] = basicRoom;

    bool *ptrs[ROOM_HEIGHT];
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        ptrs[i] = map[i];
    }

    //Shader tilesShader("shaders\\vertex\\tilesVertex.vert", "shaders\\fragment\\tilesFragment.frag");
    //Shader debugShader("shaders\\vertex\\tilesVertex.vert", "shaders\\fragment\\debugFragment.frag");
    Shader spriteShader("shaders\\vertex\\spriteVertex.vert", "shaders\\fragment\\tilesFragment.frag");
    Shader visorSprite("shaders\\vertex\\spriteVertex.vert", "shaders\\fragment\\visorSupport.frag");
    Shader visorTile("shaders\\vertex\\tilesVertex.vert", "shaders\\fragment\\visorSupport.frag");
    TileMap tileMap(ptrs, "resources\\textures\\tiles\\debug\\");
    Texture spriteSheet("resources\\spritesheet.png", 80, 16, Texture::TileMode::STRETCH, nullptr);

    glm::mat4 projection = glm::ortho(
        0.0f, (float) LOGIC_SCREEN_WIDTH,
        (float) LOGIC_SCREEN_HEIGHT, 0.0f,
        -1.0f, 1.0f);

    int meshes = tileMap.textures.size();
    float percentage = 100 - ((float) meshes / (ROOM_HEIGHT * ROOM_WIDTH) * 100);

    std::cout << "Draw calls / tiles: " << meshes << " / " << ROOM_HEIGHT * ROOM_WIDTH
    << " (%" << percentage << " calls avoided)" << std::endl;


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

    Game *game = new Game(window);

    game->start();

    delete game;


    /*
    double currentTime, lastFrame = 0;
    while (!glfwWindowShouldClose(window))
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        processInput(window, &numbers);

        glClearColor(1.0f, 1.0, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(visorTile.ID);
        glUniformMatrix4fv(glGetUniformLocation(visorTile.ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(visorTile.ID, "uRenderMode"), renderMode);
        glUniform1i(glGetUniformLocation(visorTile.ID, "uVirtualHeight"), LOGIC_SCREEN_HEIGHT);
        glUniform1i(glGetUniformLocation(visorTile.ID, "uRealHeight"), SCREEN_HEIGHT);

        tileMap.draw(false);

        glUseProgram(visorSprite.ID);
        glUniformMatrix4fv(glGetUniformLocation(visorSprite.ID, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(visorSprite.ID, "uRenderMode"), renderMode);
        glUniform1i(glGetUniformLocation(visorSprite.ID, "uVirtualHeight"), LOGIC_SCREEN_HEIGHT);
        glUniform1i(glGetUniformLocation(visorSprite.ID, "uRealHeight"), SCREEN_HEIGHT);

        numbers.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    */

    Texture::clearTextureCache();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, Sprite *sprite) {
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        sprite->playAnimation("numbers", 0);
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        sprite->stopAnimation();
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        sprite->resumeAnimation();
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        renderMode = 0;
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        renderMode = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        sprite->transform->translate2d({0, -1});
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        sprite->transform->translate2d({0, 1});
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        sprite->transform->translate2d({1, 0});
    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        sprite->transform->translate2d({-1, 0});
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCREEN_HEIGHT = width;
    SCREEN_WIDTH = height;
}

bool checkForLoadBearingSnakes() {
    std::string paths[] = {
        "download.jpg",
        "data\\7mojo6.jpg",
        "dynamic\\snake1.jpg",
        "shaders\\vertex\\photo_2025-11-07_16-36-46.jpg"
    };

    size_t sizes[] = {
        151500,
        67500,
        151296,
        167400
    };

    int width, height, channels;
    unsigned char* imageData;
    for (int i = 0; i < 4; i++) {
        imageData = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);

        if (!imageData) {
            std::cerr << "Load Bearing Snake is missing: " << paths[i] << std::endl;
            return false;
        }

        size_t size = width * height * channels * sizeof(unsigned char);

        if (size != sizes[i]) {
            std::cerr << "Load Bearing Snake is wrong size:\n" << paths[i] << " should be " << sizes[i] << " but is " << size << std::endl;
            return false;
        }

        stbi_image_free(imageData);
    }

    return true;
}