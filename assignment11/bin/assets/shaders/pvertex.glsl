#version 420
layout(location=0) in vec3 v_position;
layout(location=1) in vec3 v_color;

layout(std140, binding=1) uniform GMatrices
{
    mat4 mvpMatrix;
    mat4 mvMatrix;
    mat4 normalMatrix;
};

out vec3 f_color;

void main(void)
{
    f_color = v_color;
    gl_Position = mvpMatrix * vec4(v_position, 1.0);
}
