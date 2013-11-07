#version 420

in vec3 f_position;
in vec3 f_normal;

uniform GColors
{
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
}colors;

struct Light
{
    float intensity;
    vec3 color;
};

uniform GLights
{
    uniform Light emit;
}light;

out vec4 colout;

void main(void)
{
    colout = vec4(colors.diffuse, 1.0f) * light.emit.intensity;
}

