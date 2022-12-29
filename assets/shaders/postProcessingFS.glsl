#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D screenTexture;
layout (binding = 1) uniform sampler2D bloomBlur;

struct PostProcessing {
    bool inversion;
    bool greyscale;
    bool sharpen;
    bool blur;
    bool edge;
    bool ridge;
    bool bloom;
    float exposure;
};
uniform PostProcessing postProcessing;

const float offset = 1.0 / 300.0;
const float gamma = 2.2;

void main()
{
    // Kernel effects
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0, offset),      // top-center
        vec2(offset, offset),   // top-right
        vec2(-offset, 0.0),     // center-left
        vec2(0.0, 0.0),         // center-center
        vec2(offset, 0.0),      // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0, -offset),     // bottom-center
        vec2(offset, -offset)   // bottom-right
    );

    float kernel[9];

    if (postProcessing.sharpen) {
        kernel = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
    }
    else if (postProcessing.edge) {
        kernel = float[](
            1,  1,  1,
            1, -8,  1,
            1,  1,  1
        );
    }
    else if (postProcessing.ridge) {
        kernel = float[](
            -1, -1, -1,
            -1,  8, -1,
            -1, -1, -1
        );
    }
    else if (postProcessing.blur) {
        kernel = float[](
            1.0/16.0, 2.0/16.0, 1.0/16.0,
            2.0/16.0, 4.0/16.0, 2.0/16.0,
            1.0/16.0, 2.0/16.0, 1.0/16.0
        );
    }
    else {
        kernel = float[](
            0, 0, 0,
            0, 1, 0,
            0, 0, 0
        );
    }

    vec3 sampleTex[9];
    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        col += sampleTex[i] * kernel[i];
    }

    vec3 color = col;

    if (postProcessing.bloom) {
        vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
        color += bloomColor;
    }

    if (postProcessing.inversion) {
        color = vec3(1.0 - color);
    }
    else if (postProcessing.greyscale) {
        float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec3(average, average, average);
    }

    // exposure - tone mapping
    color = vec3(1.0) - exp(-color * postProcessing.exposure);
    // gamma correct
    color = pow(color, vec3(1.0 / gamma));

    FragColor = vec4(color, 1.0);
}