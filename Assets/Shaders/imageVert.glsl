#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 scale>

uniform mat4 projection;

out VS_OUT {
    vec2 scale;
} vs_out;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    vs_out.scale = vertex.zw;
}