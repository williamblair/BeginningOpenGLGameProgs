#version 330 core

in vec3 aPosition;
in vec2 aTexCoord0;

out vec4 vColor;
out vec2 vTexCoord0;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 pos = uView * uModel * vec4(aPosition, 1.0);

    vColor = vec4(1.0f, 1.0f, 1.0f, 0.3f); // partially transparent water
    vTexCoord0 = aTexCoord0;
    gl_Position = uProjection * pos;
}

