#version 420
layout(location=0) in vec3 v_position;
layout(location=1) in vec3 v_normal;

layout(std140, binding=1) uniform GMatrices
{
    mat4 mvpMatrix;
    mat4 mvMatrix;
    mat4 normalMatrix;
};

out vec3 f_position;
out vec3 f_normal;

void main(void)
{
    vec4 p = (mvMatrix * vec4(v_position,1.0));
    f_position = p.xyz/p.w;
    vec4 n = normalMatrix * vec4(v_normal,1.0);
    f_normal = normalize(n.xyz/n.w);
    gl_Position = mvpMatrix * vec4(v_position, 1.0);
}
