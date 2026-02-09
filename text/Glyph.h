#ifndef INVISIBLE_GLYPH_H
#define INVISIBLE_GLYPH_H
#include "glad/glad.h"

struct Glyph {
    unsigned int textureID; //OpenGL texture ID
    int width, height;      //Glyph bitmap dimensions
    int bearingX, bearingY; //Glyph offsets from baseline to left and to top
    unsigned int advance;   //Horizontal offset to advance to next glyph
    GLuint VAO, VBO, EBO;   //buffers
};

#endif //INVISIBLE_GLYPH_H