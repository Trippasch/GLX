#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_gpu_shader5 : enable

#define M_PI 3.14159265358979323846

// 1024 / (4 + 4) --> position and color
layout (points) in;
layout (invocations = 6) in;
layout (triangle_strip, max_vertices = 12) out;

layout (location = 0) uniform mat4 projView;
layout (location = 1) uniform mat4 model;

in VS_OUT
{
    vec3 Color;
    mat4 ArrowModel;
} gs_in[];

out vec3 fColor;

const float MAGNITUDE = 0.1;

const int noEdges = 6;
const float increment = 2*M_PI / float(noEdges);

void GenerateCylinder(float height, float radius)
{
    for (int i = 0; i <= noEdges; i++) {
        vec3 curXZ = vec3(radius * cos(i * increment), 0.0, radius * sin(i * increment));

        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZ, 0.0));
        EmitVertex();

        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZ.x, height, curXZ.z), 0.0));
        EmitVertex();
    }
    EndPrimitive();
}

void GenerateBottomHex(float radius)
{
    vec3 curXZ[noEdges];

    for (int i = 0; i <= noEdges; i++) {
        curXZ[i] = vec3(radius * cos(i * increment), 0.0, radius * sin(i * increment));
    }

    for (int i = noEdges / 2; i <= noEdges; i++) {
        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZ[i], 0.0));
        EmitVertex();

        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZ[noEdges-i].x, 0.0, curXZ[noEdges-i].z), 0.0));
        EmitVertex();
    }
    EndPrimitive();
}

void GenerateCone(float headHeight, float headRadius, float height)
{
    for (int i = 0; i <= noEdges; i++) {
        vec3 curXZ = vec3(headRadius * cos(i * increment), height, headRadius * sin(i * increment));

        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZ, 0.0));
        EmitVertex();

        gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(0.0, height + headHeight, 0.0), 0.0));
        EmitVertex();
    }
    EndPrimitive();
}

void Arrow(int i, float radius, float height, float headHeight, float headRadius)
{
    vec3 curXZ = vec3(radius * cos(i * increment), 0.0, radius * sin(i * increment));
    vec3 curXZCone = vec3(headRadius * cos(i * increment), height, headRadius * sin(i * increment));
    i++;
    vec3 curXZNext = vec3(radius * cos(i * increment), 0.0, radius * sin(i * increment));
    vec3 curXZConeNext = vec3(headRadius * cos(i * increment), height, headRadius * sin(i * increment));

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZ, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZNext, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZNext.x, height, curXZNext.z), 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZ, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZ.x, height, curXZ.z), 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZNext.x, height, curXZNext.z), 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZConeNext, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZCone, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(curXZ.x, height, curXZ.z), 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZConeNext, 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(vec3(0.0, height + headHeight, 0.0), 0.0));
    EmitVertex();

    gl_Position = projView * model * gs_in[0].ArrowModel * (gl_in[0].gl_Position + vec4(curXZCone, 0.0));
    EmitVertex();
}

void main()
{
    fColor = gs_in[0].Color;

    // GenerateBottomHex(0.5);
    // GenerateCylinder(10.1, 0.5);
    // GenerateCone(1.5, 1.0, 10.1);

    Arrow(gl_InvocationID, 0.5, 10.1, 1.5, 1.0);
    EndPrimitive();
}