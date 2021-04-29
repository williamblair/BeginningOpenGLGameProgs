#version 330 core

uniform sampler2D uTexture0;

in vec2 vTexCoord0;

out vec4 FragColor;

void main()
{
    vec4 outColor = texture2D(uTexture0, vTexCoord0.st);

    if (outColor.a < 0.1) {
        discard; // don't draw the current fragment
    }

    FragColor = outColor;
}

