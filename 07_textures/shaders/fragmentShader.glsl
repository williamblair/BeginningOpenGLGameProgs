#version 330 core

uniform sampler2D uTexture0;

in vec3 vColor;
in vec2 vTexCoord0;

out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0) *
                texture(uTexture0, vTexCoord0);
}

