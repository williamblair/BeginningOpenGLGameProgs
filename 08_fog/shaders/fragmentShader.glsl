#version 330 core

uniform sampler2D uTexture0;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uAmbientColor;
uniform vec4 uFogColor;

in vec3 vColor;
in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord0;
in float fogBlendFactor;

out vec4 FragColor;

// point light
vec3 calcPoint()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vPosition);
    float nDotL = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = uLightColor * vColor * texture(uTexture0, vTexCoord0).rgb * nDotL;

    return diffuse;
}

vec3 calcAmbient()
{
    return uAmbientColor * vColor * texture(uTexture0, vTexCoord0).rgb;
}

void main()
{
    //FragColor = vec4(vColor, 1.0) *
    //            texture(uTexture0, vTexCoord0);
    FragColor = vec4(calcPoint() + calcAmbient(), 1.0);

    // apply fog
    FragColor = mix(uFogColor, FragColor, fogBlendFactor);
    //FragColor = vec4(0.0, fogBlendFactor*0.5, 0.0, 1.0);
    //FragColor = vec4(fogBlendFactor, fogBlendFactor, fogBlendFactor, 1.0);
}

