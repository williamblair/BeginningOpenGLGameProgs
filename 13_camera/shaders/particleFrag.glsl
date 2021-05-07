#version 330 core

uniform sampler2D uTexture0;

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = texture(uTexture0, gl_PointCoord) * vColor;
}

