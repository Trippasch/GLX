#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 FragColor;

#define MAX_POINT_LIGHTS 10

in VS_OUT
{
    vec3 FragPos;
} fs_in;

layout (binding = 19) uniform samplerCube depthCubeMaps[MAX_POINT_LIGHTS];

uniform int nrPointLights;

struct PointLight {
    vec3 position;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform float far_plane;

void main()
{
    vec3 res = vec3(0.0);
    for (int i = 0; i < nrPointLights; i++) {
        // display closestDepth as debug (to visualize depth cubemap)
        vec3 fragToLight = fs_in.FragPos - pointLights[i].position;
        // use the light to fragment vector to sample from the depth map
        float closestDepth = texture(depthCubeMaps[i], fragToLight).r;
        // it is currently in linear range between [0,1]. Re-transform back to original value
        closestDepth *= far_plane;
        res += vec3(closestDepth / far_plane);
    }
    FragColor = vec4(res, 1.0);
}