//
// Created by clode on 09/11/2025.
//

#ifndef INVISIBLE_TEXTURE_H
#define INVISIBLE_TEXTURE_H
#include <string>
#include <unordered_map>

#include "../Shader.h"
#include "../data/structs.h"
#include "glad/glad.h"

struct TextureInfo {
    GLuint id;
    int width, height, channels;
};

class Texture {
public:
    enum TileMode {
        STRETCH,
        TILE
    };

    int imgWidth, imgHeight;

    Texture(const std::string& path, float width, float height, TileMode mode, Vec2 *staticPos);
    ~Texture();

    void draw(Vec2 pos, UVinfo *uv, bool debug);
    static void clearTextureCache();


private:
    GLuint textureID;
    std::string path;
    GLuint VAO, VBO, EBO;

    Vec2 *staticPos;

    int nrChannels;
    float width, height;
    TileMode mode;

    static std::unordered_map<std::string, TextureInfo> textureCache;

    void setupMesh();
    void loadTextureFromFile();
};


#endif //INVISIBLE_TEXTURE_H