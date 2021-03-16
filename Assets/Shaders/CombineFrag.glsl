# version 330 core
 uniform sampler2D diffuseTex ;
 uniform sampler2D diffuseLight ;
 uniform sampler2D specularLight ;
  //uniform sampler2D shadowTex ;

 in Vertex {
 vec2 texCoord ;
 } IN;

 layout (location = 0) out vec4 fragColour ;

 layout (location = 1) out vec4 bloomColour ;

 void main ( void ) {
 vec3 diffuse = texture ( diffuseTex , IN. texCoord ). xyz ;
 vec3 light = texture ( diffuseLight , IN. texCoord ). xyz ;
 vec3 specular = texture ( specularLight , IN. texCoord ). xyz ;

 fragColour .xyz = diffuse * 0.1; // ambient
 fragColour .xyz += diffuse * light ; // lambert
 fragColour .xyz += specular ; // Specular
 fragColour .a = 1.0;

 float brightness = dot(fragColour.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bloomColour = vec4(fragColour.rgb, 1.0);
	else
		bloomColour = vec4(0.0, 0.0, 0.0, 1.0);

//float temp=texture (shadowTex , IN. texCoord ).x;

 //fragColour=texture (diffuseLight , IN. texCoord );
 //fragColour=vec4(temp,0.0,0.0,1.0);
 }