#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 Texcoords;


void main(){
    Texcoords=position;//这里的顶点坐标就是cubemap采样的纹理坐标，因为天空盒就放在（0,0,0）的位置
    vec4 pos=projMatrix*viewMatrix*vec4(position,1.0);
    gl_Position=pos.xyww;
}