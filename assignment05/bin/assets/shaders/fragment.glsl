#version 420

uniform GColors
{
    vec3 color;
};

out vec4 colout;

void main(void)
{
   colout = vec4(color, 1.0);
}

