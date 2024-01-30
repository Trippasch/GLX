#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec3 Color;
    mat4 ArrowModel;
} vs_out;

void main()
{
    vec3 position = aPos;
    vec3 normal = normalize(aNormal);

    mat4 tranScaleMatrix = mat4(
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

    vs_out.ArrowModel = tranScaleMatrix * rotationMatrix;
    vs_out.Color = normal;

    gl_Position = vec4(aPos, 1.0);
}