#version 420
layout(location=2) in vec2 v_uv;
layout(location=0) in vec3 v_position;

uniform GMatrices
{
    mat4 mvpMatrix;
};

out vec2 fuv;

void main(void)
{
	fuv = v_uv;
	gl_Position = mvpMatrix * vec4(v_position, 1.0);
}   

