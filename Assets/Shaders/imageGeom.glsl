#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec2 scale;
} gs_in[];

const float aspect=0.5625;

out vec2 texCoord;

void build_rect(vec4 position)
{    
    gl_Position = position + vec4(-1.0*gs_in[0].scale.x*aspect, -1.0*gs_in[0].scale.y, 0.0, 0.0); // 1:bottom-left  
    texCoord=vec2(0.0,0.0); 
    EmitVertex();
    gl_Position = position + vec4( gs_in[0].scale.x*aspect, -1.0*gs_in[0].scale.y, 0.0, 0.0); // 2:bottom-right
    texCoord=vec2(1.0,0.0); 
    EmitVertex();
    gl_Position = position + vec4(-1.0*gs_in[0].scale.x*aspect, gs_in[0].scale.y, 0.0, 0.0); // 3:top-left
    texCoord=vec2(0.0,1.0); 
    EmitVertex();
    gl_Position = position + vec4( gs_in[0].scale.x*aspect, gs_in[0].scale.y, 0.0, 0.0); // 4:top-right
    texCoord=vec2(1.0,1.0); 
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_rect(gl_in[0].gl_Position);
}