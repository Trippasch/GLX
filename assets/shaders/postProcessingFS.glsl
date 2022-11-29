#version 330 core
#extension GL_ARB_shading_language_420pack : enable

out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D screenTexture;

struct PostProcessing {
    bool inversion;
    bool greyscale;
    bool sharpen;
    bool blur;
    bool edge;
    bool ridge;
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

    if (postProcessing.inversion) {
        FragColor = vec4(col, 1.0);
        FragColor = vec4(vec3(1.0 - FragColor), 1.0);
    }
    else if (postProcessing.greyscale) {
        FragColor = vec4(col, 1.0);
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
    else {
        FragColor = vec4(col, 1.0);
    }

    // gamma correction
    // FragColor = vec4(pow(FragColor.rgb, vec3(1.0 / gamma)), 1.0);
}