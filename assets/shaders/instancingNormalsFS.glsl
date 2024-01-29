#version 330 core
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_explicit_uniform_location : enable

layout (location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 Color;
    vec2 TexCoords;
} fs_in;

void main()
{
    FragColor = vec4(abs(fs_in.Color), 1.0);
}
