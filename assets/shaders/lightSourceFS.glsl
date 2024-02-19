#version 330 core

layout (location = 0) out vec4 FragColor;

uniform vec3 color;

in VS_OUT
{
    vec3 Normal;
} fs_in;

void main()
{
    vec3 N = normalize(fs_in.Normal);

    FragColor = vec4(color, 1.0);
    // FragColor = vec4(N, 1.0);
}