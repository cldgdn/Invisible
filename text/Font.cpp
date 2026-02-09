//
// Created by clode on 08/02/2026.
//

#include "Font.h"

#include <filesystem>
#include <iostream>

#include "glad/glad.h"

Font::Font(const std::string &fontPath, unsigned int pixelSize, FT_Library ft) :
    fontPath(fontPath),
    pixelSize(pixelSize),
    face(nullptr)
{
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "Failed to load font: " << fontPath << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, pixelSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

Font::~Font() {
    for (auto& pair : glyphs) {
        glDeleteTextures(1, &pair.second.textureID);
        glDeleteVertexArrays(pair.second.VAO, 0);
        glDeleteBuffers(1, &pair.second.VBO);
        glDeleteBuffers(1, &pair.second.EBO);
    }

    glyphs.clear();

    if (face) {
        FT_Done_Face(face);
        face = nullptr;
    }
}

const Glyph & Font::getGlyph(char c) {
    auto it = glyphs.find(c);
    if (it != glyphs.end()) {
        return it->second;
    }

    loadGlyph(c);
    return glyphs[c];
}

void Font::loadGlyph(char c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cerr << "Failed to load glyph: " << c << std::endl;
    }

    FT_GlyphSlot g = face->glyph;

    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        g->bitmap.width,
        g->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Glyph glyph;
    glyph.textureID = textureID;
    glyph.width = g->bitmap.width;
    glyph.height = g->bitmap.rows;
    glyph.bearingX = g->bitmap_left;
    glyph.bearingY = g->bitmap_top;
    glyph.advance = g->advance.x;

    float vertices[] = {
        0.0f,               0.0f,                0.0f, 0.0f, //TL
        (float)glyph.width, 0.0f,                1.0f, 0.0f, //TR
        (float)glyph.width, (float)glyph.height, 1.0f, 1.0f, //BR
        0.0f,               (float)glyph.height, 0.0f, 1.0f  //BL
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &glyph.VAO);
    glGenBuffers(1, &glyph.VBO);
    glGenBuffers(1, &glyph.EBO);

    glBindVertexArray(glyph.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, glyph.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyph.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glyphs.emplace(c, glyph);
}
