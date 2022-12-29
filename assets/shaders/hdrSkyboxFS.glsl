#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;
in vec3 WorldPos;

layout (binding = 0) uniform samplerCube environmentMap;

void main()
{
    // vec3 envColor = texture(environmentMap, WorldPos).rgb;
    vec3 envColor = textureLod(environmentMap, WorldPos, 1.2).rgb;

    FragColor = vec4(envColor, 1.0);
}
