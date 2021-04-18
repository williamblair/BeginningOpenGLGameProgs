#version 330 core

uniform sampler2D uTexture0;
uniform vec4 uFogColor;

in vec4 vColor;
in vec2 vTexCoord0;
in float fogBlendFactor;

out vec4 outColor;

void main()
{
    outColor = vColor * texture(uTexture0, vTexCoord0.st);
    outColor = mix(uFogColor, outColor, fogBlendFactor);
    //outColor = vec4(0.0, fogBlendFactor*0.5, 0.0, 1.0);
    //outColor = vec4(fogBlendFactor, fogBlendFactor, fogBlendFactor, 1.0);
}

