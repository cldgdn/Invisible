//
// Created by clode on 05/11/2025.
//

#include "Shape.h"

#include "glad/glad.h"

Shape::Shape() : vertices(NULL), indices(NULL), vCount(0), iCount(0), VAO(0), VBO(0), EBO(0) {

}

Shape::Shape(float* vertices, GLuint* indices, GLuint vCount, GLuint iCount) :
    vertices(vertices), indices(indices),
    vCount(vCount), iCount(iCount)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vCount, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * iCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Shape::~Shape() {
    //delete[] vertices;
    //delete[] indices;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
