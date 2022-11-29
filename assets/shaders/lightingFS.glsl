#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}