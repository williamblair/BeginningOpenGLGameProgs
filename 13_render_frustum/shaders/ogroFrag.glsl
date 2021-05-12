#version 330 core

uniform sampler2D uTexture0;

in vec2 vTexCoord0;

out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture0, vTexCoord0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

