#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
} fs_in;

layout (binding = 10) uniform samplerCube depthCubeMap[2];

#define NR_LIGHTS 2

struct PointLight {
    vec3 position;
};
uniform PointLight pointLights[NR_LIGHTS];

uniform float far_plane;

void main()
{
    vec3 res = vec3(0.0);
    for (int i = 0; i < NR_LIGHTS; i++) {
        // display closestDepth as debug (to visualize depth cubemap)
        vec3 fragToLight = fs_in.FragPos - pointLights[i].position;
        // use the light to fragment vector to sample from the depth map
        float closestDepth = texture(depthCubeMap[i], fragToLight).r;
        // it is currently in linear range between [0,1]. Re-transform back to original value
        closestDepth *= far_plane;
        res += vec3(closestDepth / far_plane);
    }
    FragColor = vec4(res, 1.0);
}