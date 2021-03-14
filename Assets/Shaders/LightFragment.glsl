#version 330 core

uniform vec4 color;
layout (location = 0) out vec4 fragColour;
layout (location = 1) out vec4 brightColour;

void main ( void ) {
	
    fragColour = vec4(color.xyz,1.0);

    float brightness = dot(fragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColour = vec4(fragColour.rgb, 1.0);
	else
		brightColour = vec4(0.0, 0.0, 0.0, 1.0);
}