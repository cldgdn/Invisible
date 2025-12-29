//
// Created by clode on 05/11/2025.
//

#ifndef INVISIBLE_SQUARE_H
#define INVISIBLE_SQUARE_H
#include "glad/glad.h"


class Shape {
public:
    float* vertices;
    GLuint* indices;
    GLuint vCount, iCount;
    GLuint VAO, VBO, EBO;

    Shape();
    Shape(float *vertices, GLuint *indices, GLuint vCount, GLuint iCount);
    ~Shape();
};


#endif //INVISIBLE_SQUARE_H