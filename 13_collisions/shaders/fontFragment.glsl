#version 330 core

uniform sampler2D uTexture0;

in vec2 vTexCoord0;
out vec4 FragColor;

void main(void)
{
    vec4 outColor = texture2D(uTexture0, vTexCoord0.st);

    // ignore black texels
    if (outColor.rgb == vec3(0.0, 0.0, 0.0))
    {
        discard;
    }

    FragColor = outColor;
}

