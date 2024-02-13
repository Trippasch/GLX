#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;

layout (binding = 10) uniform samplerCube depthCubeMap;

// point lights
#define NR_LIGHTS 1

struct PointLight {
    bool shadows;
    bool use;
    vec3 position;
    vec3 color;
};
uniform PointLight pointLights[NR_LIGHTS];
uniform PointLight pointLight;

uniform float far_plane;

void main()
{
    // display closestDepth as debug (to visualize depth cubemap)
    vec3 fragToLight = fs_in.FragPos - pointLights[0].position;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(depthCubeMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
}