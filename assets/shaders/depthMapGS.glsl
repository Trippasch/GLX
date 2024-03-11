#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_DIR_LIGHTS 10

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 1) uniform Lights
{
    mat4 lightSpaceMatrices[16 * MAX_DIR_LIGHTS];
};

uniform int dirLightIndex;

void main()
{
    for (int i = 0; i < 3; i++)
    {
        gl_Position = lightSpaceMatrices[dirLightIndex * 16 + gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}