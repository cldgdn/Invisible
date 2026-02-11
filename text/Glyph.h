#ifndef INVISIBLE_GLYPH_H
#define INVISIBLE_GLYPH_H


struct Glyph {
    unsigned int textureID; //OpenGL texture ID
    int width, height;      //Glyph bitmap dimensions
    int bearingX, bearingY; //Glyph offsets from baseline to left and to top
    unsigned int advance;   //Horizontal offset to advance to next glyph
    unsigned int VAO, VBO, EBO;   //buffers
};

#endif //INVISIBLE_GLYPH_H