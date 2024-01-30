#version 330 core
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 0) uniform mat4 projView;
layout (location = 1) uniform mat4 model;

out VS_OUT
{
    vec3 Normal;
} vs_out;

void main()
{
    vs_out.Normal = mat3(model) * aNormal;
    gl_Position = projView * model * vec4(aPos, 1.0);
}