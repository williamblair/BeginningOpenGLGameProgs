#version 330 core

in vec3 aPosition;
in vec2 aTexCoord0;

out vec4 vColor;
out vec2 vTexCoord0;
out float fogBlendFactor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

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
    vec4 pos = uView * uModel * vec4(aPosition, 1.0);

    setFogBlendFactor(vec3(pos));

    vColor = vec4(1.0f, 1.0f, 1.0f, 0.3f); // partially transparent water
    vTexCoord0 = aTexCoord0;
    gl_Position = uProjection * pos;
}

