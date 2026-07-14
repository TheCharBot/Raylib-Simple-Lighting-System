#version 330

#define MAX_LIGHTS 80

in vec2 fragTexCoord;
out vec4 finalColor;

uniform vec2 lightPositions[MAX_LIGHTS];
uniform float lightRadii[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensity[MAX_LIGHTS];
uniform float lightSoftness[MAX_LIGHTS];
uniform float lightBrightness[MAX_LIGHTS];

uniform int lightCount;

uniform float aspectRatio;

// Amount of darkness everywhere (0 = no darkness, 1 = pitch black)
uniform float ambientDarkness;

void main()
{
    vec2 uv = fragTexCoord;

    // Correct aspect ratio
    uv.x *= aspectRatio;

    float brightness = 0.0;
    float colorBrightness = 0.0;
    vec3 lightColor = vec3(0.0);

    for(int i = 0; i < lightCount; i++)
    {
        vec2 light = lightPositions[i];
        light.x *= aspectRatio;

        vec2 d = uv - light;
        float dist2 = dot(d, d);

        float radius = lightRadii[i];
        float soft = clamp(lightSoftness[i], 0.001, 0.999);

        float inner = radius * (1.0 - soft);

        float radius2 = radius * radius;
        float inner2 = inner * inner;

        if(dist2 >= radius2)
            continue;

        float falloff = 1.0 - smoothstep(inner2, radius2, dist2);

        float contribution = falloff * lightIntensity[i];

        brightness += contribution;

        if(dot(lightColors[i], lightColors[i]) > 0.000001)
        {
            float colorContribution = contribution * lightBrightness[i];

            lightColor += lightColors[i] * colorContribution;
            colorBrightness += colorContribution;
        }
    }

    brightness = clamp(brightness, 0.0, 1.0);

    if(colorBrightness > 0.0001)
        lightColor /= colorBrightness;
    else
        lightColor = vec3(0.0);

    float darkness = ambientDarkness * (1.0 - brightness);

    vec3 tint = lightColor * colorBrightness;

    finalColor = vec4(tint, darkness);
}