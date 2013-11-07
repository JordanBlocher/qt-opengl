#version 420

in vec2 f_uv;
in vec3 f_position;
in vec3 f_normal;

uniform GColors
{
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
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

uniform sampler2D diffuseTexture;

out vec4 f_out;

void main(void)
{
    vec4 colout = vec4(colors.diffuse, 1.0f)  * light.emit.intensity;
    f_out =  texture2D(diffuseTexture, f_uv.xy) * light.emit.intensity;
}

