#version 330 core
 uniform mat4 modelMatrix ;
 uniform mat4 viewMatrix ;
 uniform mat4 projMatrix ;

 in vec3 position ;
 in vec3 normal ;
 in vec2 texCoord ;

 out Vertex {
 vec2 texCoord ;
 vec3 normal ;
 } OUT;

 void main ( void ) {
 OUT . texCoord = texCoord ;
 mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

 vec3 wNormal = normalize ( normalMatrix * normalize ( normal ));//得到可以使用的法线

 OUT . normal = wNormal ;

 gl_Position = ( projMatrix * viewMatrix ) * worldPos ;
 }