# version 330 core
 uniform sampler2D diffuseTex ;
  uniform sampler2D bloomTex ;
uniform float exposure ;
 in Vertex {
 vec2 texCoord ;
 } IN;

 out vec4 fragColour ;

 void main ( void ) {
 	const float gamma = 2.2;
 	//const float exposure = 0.85;
 	vec3 color = texture ( diffuseTex , IN. texCoord ).rgb ;
 	vec3 bloom=texture ( bloomTex , IN. texCoord ).rgb ;
 	color+=bloom;

 	vec3 result = vec3(1.0) - exp(-color * 0.1);
 	result = pow(result, vec3(1.0 / gamma));
 	fragColour=vec4(result,1.0);
 }