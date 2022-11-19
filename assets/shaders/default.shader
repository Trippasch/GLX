#shader vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_mvp;

void main()
{
    gl_Position = /*u_mvp * */vec4(a_Position, 1.0f);
    v_TexCoord = a_TexCoord;
}

#shader fragment
#version 330 core

in vec2 v_TexCoord;

out vec4 o_Color;

uniform usampler2D u_Tex;

void main()
{
    vec4 c = texture(u_Tex, v_TexCoord);
    o_Color = c / 255.0;
}