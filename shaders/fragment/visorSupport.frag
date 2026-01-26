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
    //Darkness mode -> each fragment is set to be a shade of gray based on the sampled color's brightness
    else if (uRenderMode == 2) {
        float brightness = ((sampledColor.x + sampledColor.y + sampledColor.z) / 3);

        brightness = clamp(brightness, 0.0, 1.0);

        vec3 color = mix(
            vec3(0.0f, 0.0f, 0.0f),
            vec3(0.15f, 0.15f, 0.15f),
            brightness
        );

        FragColor = vec4(color, sampledColor.a);
    }
    //NVG in bright area mode -> same as normal NVG but brightness is bumped up.
    else if (uRenderMode == 3) {
        float brightness = ((sampledColor.x + sampledColor.y + sampledColor.z) / 3) + 0.3;

        float row = floor((gl_FragCoord.y / uRealHeight) * uVirtualHeight);
        float scanline = (mod(row, 2.0) == 0.0) ? 0.85 : 1.0;
        brightness *= scanline;

        brightness = clamp(brightness, 0.0, 1.0);

        vec3 color = mix(
            vec3(0.3f, 0.5f, 0.3f),
            vec3(0.1f, 1.0f, 0.1f),
            brightness
        );

        FragColor = vec4(color, sampledColor.a);
    }
    //Standard rendering (no visor active)
    else {
            FragColor = sampledColor;
    }


}