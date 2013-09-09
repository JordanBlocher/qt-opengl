#version 420
in vec3 v_position;
in vec3 v_color;
out vec3 color;
uniform GMatrices
{
    mat4 mvpMatrix;
};
void main(void){
   gl_Position = mvpMatrix * vec4(v_position, 1.0);
   color = v_color;
}
