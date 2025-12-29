#version 330 core

uniform sampler2D uTexture;
uniform vec2 uTileSize;

out vec4 FragColor;

in vec2 texCoord;

void main() {
    FragColor = texture(uTexture, texCoord);
}