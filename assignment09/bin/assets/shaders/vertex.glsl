#version 420
layout(location=0) in vec3 v_position;

uniform GMatrices
{
    mat4 mvpMatrix;
};

void main(void)
{
   gl_Position = mvpMatrix * vec4(v_position, 1.0);
}
