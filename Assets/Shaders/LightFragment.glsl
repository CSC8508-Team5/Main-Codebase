#version 330 core

uniform vec4 color;
out vec4 fragColour;
//layout (location = 1) out vec4 brightColour;

void main ( void ) {
	
    fragColour = vec4(color.xyz,1.0);
    //brightColour=fragColour;
}