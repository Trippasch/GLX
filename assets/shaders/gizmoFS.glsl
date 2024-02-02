#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

uniform int axis;

void main()
{
    if (axis == 0) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if (axis == 1) {
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if (axis == 2) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}