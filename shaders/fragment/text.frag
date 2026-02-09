#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uText;
uniform vec4 uTextColor;

void main() {
    float glyphAlpha = texture(uText, vTexCoord).r;
    FragColor = vec4(uTextColor.rgb, uTextColor.a * glyphAlpha);
}