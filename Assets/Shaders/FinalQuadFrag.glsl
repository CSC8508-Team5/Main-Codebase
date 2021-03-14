# version 330 core
 uniform sampler2D diffuseTex ;
  uniform sampler2D bloomTex ;

 in Vertex {
 vec2 texCoord ;
 } IN;

 out vec4 fragColour ;

 void main ( void ) {
 	vec3 color = texture ( diffuseTex , IN. texCoord ).rgb ;
 	vec3 bloom=texture ( bloomTex , IN. texCoord ).rgb ;
 	color+=bloom;
 	fragColour=vec4(color,1.0);
 }