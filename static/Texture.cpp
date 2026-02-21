//
// Created by clode on 09/11/2025.
//

#include "Texture.h"

#include <complex>
#include <iostream>

#include "stb_image.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

std::unordered_map<std::string, TextureInfo> Texture::textureCache;

/*
 * Creates a new Texture object based on file path and size. Allows to choose whether to tile or stretch and if to always draw it at the exact same spot.
 */
Texture::Texture(const std::string &path, float width, float height, TileMode mode, Vec2 *staticPos) :
    width(width), height(height), mode(mode), staticPos(staticPos), path(path)
{
    auto it = textureCache.find(path);
    if (it != textureCache.end()) {
        const TextureInfo &info = it->second;
        textureID = info.id;
        imgWidth = info.width;
        imgHeight = info.height;
        nrChannels = info.channels;
        //std::cout << "[Texture] Reusing chached texture: " << path << std::endl;
    } else {
        loadTextureFromFile();
        textureCache[path] = {textureID, imgWidth, imgHeight, nrChannels};
        if (path == "resources/textures/props/background_camo.png")
            std::cout << "[Texture] Loaded new texture: " << path << " " << imgWidth << " " << imgHeight << " " <<  std::endl;
    }

    setupMesh();
}

Texture::~Texture() {
    delete staticPos;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

/*
 * Loads the new image from a file
 */
void Texture::loadTextureFromFile() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint wrapMode = (mode == TileMode::TILE) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

    if (data) {
        GLint format;
        switch (nrChannels) {
            case 1: format = GL_RED; break;
            case 3: format = GL_RGB; break;
            case 4: format = GL_RGBA; break;
            default: format = GL_RGB; break;
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "[Texture] Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
}

/*
 * generates the quad mesh based on the intended screen size of the texture
 */
void Texture::setupMesh() {
    float uMax, vMax;

    switch (mode) {
        case TileMode::TILE:
            uMax = width / imgWidth;
            vMax = height / imgHeight;
            break;
        case TileMode::STRETCH:
            uMax = 1.0f;
            vMax = 1.0f;
            break;
    }

    float vertices[] = {
        0.0f, 0.0f, 0.0f,       0.0f, vMax, //top-left corner
        width, 0.0f, 0.0f,      uMax, vMax, //top-right corner
        width, height, 0.0f,    uMax, 0.0f, //bottom-right corner
        0.0f, height, 0.0f,     0.0f, 0.0f  //bottom-left corner
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Texture::draw(Vec2 pos, UVinfo *uv, bool debug) {
    if (staticPos != nullptr) {
        pos = *staticPos;
    }

    glBindVertexArray(VAO);

    GLint programID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f));

    GLint modelLoc = glGetUniformLocation(programID, "uModel");
    GLint tileSizeLoc = glGetUniformLocation(programID, "uTileSize");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    if (uv == nullptr) {
        glUniform2f(tileSizeLoc, imgWidth, imgHeight);
    } else {
        glUniform2f(tileSizeLoc, uv->width, uv->height);
    }


    if (debug) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.5f);

        GLint colorLoc = glGetUniformLocation(programID, "uWireColor");
        if (colorLoc != -1) {
            glUniform3f(colorLoc, 1.0f, 0.6f, 0.0f);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        GLint textureLoc = glGetUniformLocation(programID, "uTexture");
        glUniform1i(textureLoc, 0);

        if (mode == TileMode::STRETCH) {
            GLint frameOffsetLoc = glGetUniformLocation(programID, "uFrameOffset");
            GLint frameScaleLoc = glGetUniformLocation(programID, "uFrameScale");

            if (uv != nullptr) {
                glUniform2f(frameOffsetLoc, uv->texCoord.x / imgWidth, uv->texCoord.y / imgHeight);
                glUniform2f(frameScaleLoc, uv->width / imgWidth, uv->height / imgHeight);
            } else {
                glUniform2f(frameOffsetLoc, 0.0f, 0.0f);
                glUniform2f(frameScaleLoc, 1.0f, 1.0f);
            }
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
}

void Texture::clearTextureCache() {
    for (auto &[path, info] : textureCache) {
        glDeleteTextures(1, &info.id);
    }
    textureCache.clear();
}
