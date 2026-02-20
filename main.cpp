#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <glm/glm.hpp>

#include "Game.h"
#include "glm/gtc/type_ptr.hpp"
#include "static/Texture.h"
#include "globals.h"
#include "Model.h"
#include "dynamic/Sprite.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
bool checkForLoadBearingSnakes();

std::string loadShaderSource(const char* filePath);
GLuint compileShader(const char* filePath, GLenum shaderType);

// settings
unsigned int SCREEN_WIDTH = 1600;
unsigned int SCREEN_HEIGHT = 900;

unsigned int LOGIC_SCREEN_WIDTH = 32 * TILE_SIZE;
unsigned int LOGIC_SCREEN_HEIGHT = 18 * TILE_SIZE;

double deltaTime = 0, scrollX = 0, scrollY = 0;
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
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Invisible - Game", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "GL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    Game *game = new Game(window);

    game->start();

    delete game;

    Texture::clearTextureCache();
    glfwTerminate();
    return 0;
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

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    scrollY = yOffset;
    scrollX = xOffset;
}

bool checkForLoadBearingSnakes() {
    std::string paths[] = {
        "resources/loadbearing/download.jpg",
        "resources/loadbearing/7mojo6.jpg",
        "resources/loadbearing/snake1.jpg",
        "resources/loadbearing/photo_2025-11-07_16-36-46.jpg"
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