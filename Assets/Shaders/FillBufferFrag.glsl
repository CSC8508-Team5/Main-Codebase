# version 330 core

 uniform sampler2D diffuseTex ; 

 in Vertex {
	vec2 texCoord ;
 	vec3 normal ;
 } IN;

 out vec4 fragColour [2]; // MRT

 void main ( void ) {
 fragColour [0] = texture ( diffuseTex , IN. texCoord );
 fragColour [1] = vec4 ( IN.normal .xyz * 0.5 + 0.5 ,1.0);
 }