//
// Created by clode on 08/02/2026.
//

#include "TextObject.h"

#include <vector>

#include "Font.h"
#include "Glyph.h"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

TextObject::TextObject(std::shared_ptr<Font> font, const std::string &text) :
    font(std::move(font)),
    text(text),
    VAO(0),
    VBO(0),
    vertexCount(0)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //rebuildMesh();
}

TextObject::~TextObject() {
    if (VAO != 0)
        glDeleteVertexArrays(1, &VAO);
    if (VBO != 0)
        glDeleteBuffers(1, &VBO);
}

void TextObject::setText(const std::string &text) {
    if (this->text == text) {
        return;
    }

    this->text = text;
    //rebuildMesh();
}

const std::string & TextObject::getText() const {
    return this->text;
}

float TextObject::getWidth() {
    if (width < 0) {
        width = 0;
        float x = 0;

        const Glyph &first = font->getGlyph(text[0]);
        float left = first.bearingX;

        for (char c : this->text) {
            const Glyph &glyph = font->getGlyph(c);
            x += (glyph.advance >> 6);
        }

        const Glyph &last = font->getGlyph(text.back());
        float right = x - (last.advance >> 6) + last.bearingX + last.width;
        width = std::ceil(right - left);
    }

    return width;
}

float TextObject::getHeight() {
    if (height < 0) {
        height = 0;

        for (char c : this->text) {
            const Glyph &glyph = font->getGlyph(c);
            if (height < glyph.height) {
                height = glyph.height;
            }
        }
    }

    return height;
}

void TextObject::draw() {
    float cursor = position.x;
    float baseline = 0;

    GLint programID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

    glUniform4f(glGetUniformLocation(programID, "uTextColor"), color.r, color.g, color.b, color.a);

    for (char c : this->text) {
        const Glyph& glyph = font->getGlyph(c);
        baseline = std::max((float) glyph.bearingY, baseline);
    }

    baseline = position.y + baseline * scale;

    for (char c : text) {
        const Glyph& glyph = font->getGlyph(c);
        glBindVertexArray(glyph.VAO);



        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                cursor + glyph.bearingX * scale,
                baseline - glyph.bearingY * scale,
                0.0f
            )
        );
        model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

        GLint modelLoc = glGetUniformLocation(programID, "uModel");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glyph.textureID);

        GLint textLoc = glGetUniformLocation(programID, "uText");
        glUniform1i(textLoc, 0);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        cursor += (glyph.advance >> 6) * scale;
    }


    /*glBindVertexArray(VAO);

    unsigned int offset = 0;
    float xCursor = position.x;

    GLint programID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programID);

    GLint modelLoc = glGetUniformLocation(programID, "uModel");

    for (char c : text) {
        const Glyph& g = font->getGlyph(c);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g.textureID);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(xCursor, position.y, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, offset, 6);  // draw single glyph quad
        glBindVertexArray(0);

        offset += 6;
        xCursor += (g.advance >> 6);
    }

    glBindVertexArray(0);*/
}

void TextObject::rebuildMesh() {
    std::vector<float> vertices;
    vertices.reserve(text.size() * 6 * 4);

    float x = 0.0f;
    float y = 0.0f;

    for (char c : text) {
        const Glyph& g = font->getGlyph(c);

        float xPos = x + g.bearingX;
        float yPos = y + g.bearingY;

        float w = g.width;
        float h = g.height;

        vertices.insert(vertices.end(), {
            //tri 1
            xPos,     yPos - h,   0.0f, 0.0f,
            xPos,     yPos,       0.0f, 1.0f,
            xPos + w, yPos,       1.0f, 1.0f,
            //tri 2
            xPos,     yPos - h,   0.0f, 0.0f,
            xPos + w, yPos,       1.0f, 1.0f,
            xPos + w, yPos - h,   1.0f, 0.0f
        });

        x += (g.advance >> 6);
    }

    vertexCount = vertices.size() / 4;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );


    glVertexAttribPointer(
        0, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
