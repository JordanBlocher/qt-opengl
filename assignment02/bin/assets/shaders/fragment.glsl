#version 420
in vec3 color;
void main(void){
   gl_FragColor = vec4(color.rgb, 1.0);
}
