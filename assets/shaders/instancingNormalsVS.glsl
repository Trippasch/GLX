#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_packing : enable

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec4 aInstanceVec;

out VS_OUT
{
    vec3 Color;
    vec2 TexCoords;
} vs_out;

layout (location = 0) uniform mat4 projView;
layout (location = 1) uniform mat4 model;

void main()
{
    vec3 position = aInstanceVec.xyz;
    vec3 normal = unpackSnorm4x8(floatBitsToUint(aInstanceVec.w)).xyz;

    mat4 instanceMatrix = mat4(
        vec4(0.01, 0.0, 0.0, 0.0),
        vec4(0.0, 0.01, 0.0, 0.0),
        vec4(0.0, 0.0, 0.01, 0.0),
        vec4(position, 1.0));

    vec3 orig = vec3(0.0, 1.0, 0.0);
    vec3 crossP = cross(orig, normal);
    float dotP = dot(orig, normal);
    float angle = acos(dotP / (length(orig) * length(normal)));

    mat4 rotationMatrix = mat4(
        vec4((cos(angle) + (crossP.x*crossP.x)*(1-cos(angle))),
            (crossP.y*crossP.x*(1 - cos(angle)) + crossP.z*sin(angle)),
            (crossP.z*crossP.x*(1 - cos(angle)) - crossP.y*sin(angle)), 0.0),
        vec4((crossP.x*crossP.y*(1 - cos(angle)) - crossP.z*sin(angle)),
            (cos(angle) + (crossP.y*crossP.y)*(1 - cos(angle))),
            (crossP.z*crossP.y*(1 - cos(angle)) + crossP.x*sin(angle)), 0.0),
        vec4((crossP.x*crossP.z*(1 - cos(angle)) + crossP.y*sin(angle)),
            (crossP.y*crossP.z*(1 - cos(angle)) - crossP.x*sin(angle)),
            (cos(angle) + (crossP.z*crossP.z)*(1 - cos(angle))), 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));

    vs_out.TexCoords = aTexCoords;
    vs_out.Color = normal;

    gl_Position = projView * model * instanceMatrix * rotationMatrix * vec4(aPos, 1.0);
}