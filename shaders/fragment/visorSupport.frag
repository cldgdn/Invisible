#version 330 core

uniform sampler2D uTexture;
uniform vec2 uTileSize;
uniform int uRenderMode;
uniform int uVirtualHeight;
uniform int uRealHeight;

out vec4 FragColor;

in vec2 texCoord;

void main() {
    vec4 sampledColor = texture(uTexture, texCoord);

    //Night Vision mode -> each fragment is set to be green of the same intensity as the sampled color's brightness
    if (uRenderMode == 1) {
        float brightness = (sampledColor.x + sampledColor.y + sampledColor.z) / 3;

        float row = floor((gl_FragCoord.y / uRealHeight) * uVirtualHeight);
        float scanline = (mod(row, 2.0) == 0.0) ? 0.85 : 1.0;
        brightness *= scanline;

        brightness = clamp(brightness, 0.0, 1.0);

        vec3 color = mix(
            vec3(0.0f, 0.1f, 0.0f),
            vec3(0.7f, 1.0f, 0.7f),
            brightness
        );

        FragColor = vec4(color, sampledColor.a);
    }
    //Standard rendering (no visors active)
    else {
            FragColor = sampledColor;
    }


}