#version 330 core

attribute vec3 aPosition;
attribute vec2 aTexCoord0;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 vTexCoord0;

void main(void)
{
    vec4 pos = uView * uModel * vec4(aPosition, 1.0);
    vTexCoord0 = aTexCoord0;
    gl_Position = uProjection * pos;
}

