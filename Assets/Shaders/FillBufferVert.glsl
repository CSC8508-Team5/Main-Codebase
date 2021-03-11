#version 330 core
 uniform mat4 modelMatrix ;
 uniform mat4 viewMatrix ;
 uniform mat4 projMatrix ;
 uniform mat4 shadowMatrix 	;


layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;

uniform vec4 		objectColour = vec4(1,1,1,1);

uniform bool hasVertexColours = false;

 out Vertex {
 vec2 texCoord ;
 vec3 normal ;
 vec4 shadowProj;
 vec4 colour;
 } OUT;

 void main ( void ) {
 OUT . texCoord = texCoord ;
 mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));
OUT.shadowProj 	=  shadowMatrix *modelMatrix* vec4 ( position,1);
 vec3 wNormal = normalize ( normalMatrix * normalize ( normal ));

 OUT . normal = wNormal ;

 OUT.colour		= objectColour;

if(hasVertexColours) {
	OUT.colour		= objectColour * colour;
}

 vec4 worldPos = ( modelMatrix * vec4 ( position ,1));

 gl_Position = ( projMatrix * viewMatrix ) * worldPos ;
 }