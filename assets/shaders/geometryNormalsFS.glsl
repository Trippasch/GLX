#version 330 core
out vec4 FragColor;

in vec3 fColor;

void main()
{
    FragColor = vec4(abs(fColor), 1.0);
    // FragColor = vec4(0.5, 1.0, 0.5, 1.0);
}