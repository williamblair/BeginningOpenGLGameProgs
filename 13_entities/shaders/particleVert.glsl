#version 330 core

in vec3 aPosition;
in vec4 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uPointSize;

out vec4 vColor;

void main()
{
    vec4 pos = uView * uModel * vec4(aPosition, 1.0);

    gl_PointSize = uPointSize;
    gl_Position = uProjection * pos;

    vColor = aColor;
}

