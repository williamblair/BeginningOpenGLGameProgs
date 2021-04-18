#version 330 core

in vec3 aPosition;
in vec3 aNormal;
in vec3 aColor;
in vec2 aTexCoord0;

out vec3 vColor;
out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoord0;
out float fogBlendFactor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uNormal; // should be the transpose of the inverse of the model matrix

uniform float uFogStart;
uniform float uFogEnd;
uniform float uFogDensity;
uniform int uFogType;

void setFogBlendFactor(vec3 modelViewPos)
{
    if (uFogType == 0) // linear
    {
        fogBlendFactor = clamp((uFogEnd - length(modelViewPos)) / (uFogEnd - uFogStart), 0.0, 1.0);
    }
    else if (uFogType == 1) // exponential
    {
        fogBlendFactor = exp(-uFogDensity * length(modelViewPos));
    }
    else // exponential squared
    {
        fogBlendFactor = exp2(-uFogDensity * length(modelViewPos));
    }
}

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);

    setFogBlendFactor(vec3(uView * uModel * vec4(aPosition, 1.0)));

    vColor = aColor;
    vTexCoord0 = aTexCoord0;
    vPosition = vec3(uModel * vec4(aPosition,1.0)); // world space vertex coord
    vNormal = normalize(vec3(uNormal * vec4(aNormal,1.0)));
}

