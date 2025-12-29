//
// Created by clode on 07/11/2025.
//

#ifndef INVISIBLE_SHADER_H
#define INVISIBLE_SHADER_H
#include "glad/glad.h"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class Shader {
public:
    GLuint ID;

    Shader(const char *vertexPath, const char *fragmentPath);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

private:
    void checkCompileErrors(GLuint shader, std::string type) const;
};


#endif //INVISIBLE_SHADER_H