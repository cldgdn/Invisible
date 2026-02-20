#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform Material material;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 diffuseTex = texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specularTex = texture(material.texture_specular1, TexCoords).rgb;

    // ambient
    vec3 ambient = 0.2 * diffuseTex;

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseTex;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * specularTex;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
