#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D diffTex;

uniform vec3 color;



void main()
{
    FragColor = texture ( diffTex , texCoord);
    FragColor *= vec4(color,1.0);
}