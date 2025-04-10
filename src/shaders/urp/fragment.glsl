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
    sampler2D occlusion_map;
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

uniform bool use_base_map;
uniform bool use_normal_map;
uniform bool use_occlusion_map;

void main()
{
    // --- Normal Calculation ---
    vec3 normal = normalize(Normal);
    if (use_normal_map) {
        vec3 T = normalize(Tangent);
        vec3 B = normalize(Bitangent);
        vec3 N = normalize(Normal);
        mat3 TBN = mat3(T, B, N);

        vec3 normalSample = texture(material.normal_map, TexCoords).rgb;
        normalSample = normalize(normalSample * 2.0 - 1.0);
        normal = normalize(TBN * normalSample);
    }

    // --- Base Color ---
    vec3 color = material.base_color;
    if (use_base_map) {
        vec4 baseTex = texture(material.base_map, TexCoords);
        color *= baseTex.rgb;
    }

    // --- Metallic ---
    float metallic = texture(material.metallic_map, TexCoords).r;

    // --- Occlusion ---
    float occlusion = 1.0;
    if (use_occlusion_map) {
        occlusion = texture(material.occlusion_map, TexCoords).r;
    }

    // --- Lighting ---
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Ambient
    vec3 ambient = light.ambient * color * occlusion;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * color;

    // Specular (simple PBR-inspired Fresnel)
    vec3 specColor = mix(vec3(0.04), color, metallic);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.smoothness);
    vec3 specular = light.specular * spec * specColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
