#version 330 core

uniform sampler2D uTexture0;

in vec4 vColor;
in vec2 vTexCoord0;

out vec4 outColor;

void main()
{
    outColor = vColor * texture(uTexture0, vTexCoord0.st);
}

