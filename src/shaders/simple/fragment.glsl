#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

struct Material {
    vec3 base_color;
};

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    vec3 normal = normalize(Normal);

    vec3 color = material.base_color;
    vec3 ambient = light.ambient * color;


    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * color;

    FragColor = vec4(ambient + diffuse, 1.0);
}