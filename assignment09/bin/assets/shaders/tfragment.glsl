#version 420

in vec2 fuv;

uniform sampler2D diffuseTexture;

out vec4 fout;

void main(void)
{
   fout = texture2D(diffuseTexture, fuv.xy);
}

