#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;

struct Material {
    vec3 base_color;
    sampler2D base_map;
    sampler2D metallic_map;
    sampler2D normal_map;
    // sampler2D height_map;
    // sampler2D occlusion_map;
    float smoothness;
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
    if (texture(material.normal_map, TexCoords).r != 0.0)
    {
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, B, N);
        normal = texture(material.normal_map, TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    vec3 color = material.base_color;
    if (texture(material.base_map, TexCoords).a != 0.0) {
        color *= texture(material.base_map, TexCoords).rgb;
    }
    // float metallic = texture(material.metallic_map, TexCoords).r;
    // float occlusion = texture(material.occlusion_map, TexCoords).r;

    // Ambient
    // vec3 ambient = light.ambient * color * occlusion;
    vec3 ambient = light.ambient * color;


    // Diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * color;

    // // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.smoothness);
    vec3 specular = light.specular * spec * vec3(0.5);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    // FragColor = vec4(ambient + diffuse + specular, 1.0);
}