#version 330 core

 uniform mat4 modelMatrix ;
 uniform mat4 viewMatrix ;
 uniform mat4 projMatrix ;

uniform int lightType;

 uniform float lightRadius ;
 uniform vec3 lightPos ;

 layout (location = 0) in vec3 position ;


void main(void) {
	if(lightType==0){
		gl_Position =  vec4 ( position , 1.0);
	}else if(lightType==1){
		vec3 scale = vec3 ( lightRadius );
 		vec3 worldPos = ( position * scale ) + lightPos ;//注意先缩放再平移，就是把pointlight变成我们想要的大小
 		gl_Position = ( projMatrix * viewMatrix ) * vec4 ( worldPos , 1.0);
	}


}