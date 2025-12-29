#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uProjection;

uniform vec2 uFrameOffset;
uniform vec2 uFrameScale;

out vec2 texCoord;

void main() {
    gl_Position = uProjection * uModel * vec4(aPos, 1.0f);

    texCoord = aTexCoord * uFrameScale + uFrameOffset;
}