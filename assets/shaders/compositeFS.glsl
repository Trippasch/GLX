#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) out vec4 FragColor;

// color accumulation buffer
layout (binding = 2) uniform sampler2DMS accum;
// revealage threshold buffer
layout (binding = 3) uniform sampler2DMS reveal;

// epsilon number
const float EPSILON = 0.00001;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v) 
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    // fragment coordination
    ivec2 coords = ivec2(gl_FragCoord.xy);

    vec4 accumulation = vec4(0.0);
    float revealage = 0.0;
    for (int i = 0; i < 4; i++) {
        // fragment color
        accumulation += texelFetch(accum, coords, i);
        // fragment revealage
        revealage += texelFetch(reveal, coords, i).r;
    }
    accumulation /= 4.0;
    revealage /= 4.0;

    // save the blending and color texture fetch cost if there is not a transparent fragment
    if (isApproximatelyEqual(revealage, 1.0)) 
        discard;

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb)))) 
        accumulation.rgb = vec3(accumulation.a);

    // prevent floating point precision bug
    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    // blend pixels
    FragColor = vec4(average_color, 1.0 - revealage);
}