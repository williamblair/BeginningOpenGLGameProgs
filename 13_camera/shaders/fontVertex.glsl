#version 330 core

in vec2 aPosition;
in vec2 aTexCoord0;

uniform vec2 uTranslate;

out vec2 vTexCoord0;

void main()
{
    vTexCoord0 = aTexCoord0;
    gl_Position = vec4(aPosition + uTranslate,
                       0.0,
                       1.0);
}
