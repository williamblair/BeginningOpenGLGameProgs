#version 330 core

in vec3 aPosition;
in vec3 aColor;
in vec2 aTexCoord0;

out vec3 vColor;
out vec2 vTexCoord0;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);

    vColor = aColor;
    vTexCoord0 = aTexCoord0;
}

