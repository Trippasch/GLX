#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2DArray depthMap;

uniform float nearPlane;
uniform float farPlane;
uniform int layer;

// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depthValue = texture(depthMap, vec3(TexCoords, layer)).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); // perspective
    FragColor = vec4(vec3(depthValue), 1.0); // orthographic
}