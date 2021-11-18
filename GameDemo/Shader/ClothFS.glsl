#version 330 core

out vec4 color;

in vec3 position;
in vec2 texCoord;
in vec3 normal;

// Texture Sampler
uniform sampler2D uniTex;
uniform sampler2D emissionTex;

uniform vec3 uniLightPos;
uniform vec3 uniLightColor;
uniform float time;

void main()
{
    // Ambient
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * uniLightColor;

    // Diffuse
    vec3 lightDir = normalize(uniLightPos - position);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * uniLightColor;

    // texture() will output the color obtained by sampling the texture with configured conditions
    color = texture(uniTex, texCoord);
    vec3 objectColor = vec3(color.x, color.y, color.z);

	emission = (sin(time) / 2.0 + 0.5) * texture(emissionTex, texCoord + vec2(0.0, time)).rgb;

    vec3 result = (ambient + diffuse + emission) * objectColor;
    color = vec4(result, 1.0f);
}
