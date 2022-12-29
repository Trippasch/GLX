#version 330 core
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;

void main()
{
    // vec3 envColor = texture(environmentMap, WorldPos).rgb;
    vec3 envColor = textureLod(environmentMap, WorldPos, 1.2).rgb;

    FragColor = vec4(envColor, 1.0);

    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
