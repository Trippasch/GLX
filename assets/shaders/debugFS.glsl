#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D debugTexture;

void main()
{
    FragColor = texture(debugTexture, TexCoords);
}