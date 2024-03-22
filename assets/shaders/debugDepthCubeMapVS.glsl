#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 aPos;
layout (location = 5) in ivec4 aBoneIDs;
layout (location = 6) in vec4 aWeights;

out VS_OUT
{
    vec3 FragPos;
} vs_out;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projView;
    mat4 finalBonesMatrices[MAX_BONES];
    bool hasBones;
};
layout (location = 1) uniform mat4 model;

void main()
{
    vec4 totalPosition = vec4(0.0);
    if (hasBones) {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
            if(aBoneIDs[i] == -1) {
                continue;
            }
            if(aBoneIDs[i] >= MAX_BONES) {
                totalPosition = vec4(aPos, 1.0);
                break;
            }
            vec4 localPosition = finalBonesMatrices[aBoneIDs[i]] * vec4(aPos, 1.0);
            totalPosition += localPosition * aWeights[i];
        }
    }
    else {
        totalPosition = vec4(aPos, 1.0);
    }

    vs_out.FragPos = vec3(model * totalPosition);

    gl_Position = projView * vec4(vs_out.FragPos, 1.0);
}