# version 330 core

 uniform sampler2D depthTex ;
 uniform sampler2D normTex ;
  uniform sampler2D shadowTex ;

 uniform vec2 pixelSize ; // reciprocal of resolution
 uniform vec3 cameraPos ;
 uniform int lightType;

struct DirectionalLight{
    vec3 direction;
    float ambient;
    vec4 color;
};

struct PointLight{
    float ambient;
    vec4 color;
    vec3 position;
    float radius ;
};

uniform DirectionalLight directionalLight;
uniform PointLight pointLight;

 uniform mat4 inverseProjView ;

  layout (location = 0) out vec4 diffuseOutput ;
 layout (location = 1) out vec4 specularOutput ;


 vec2 CalculateDirectionalLight(DirectionalLight light, vec3 viewDir, vec3 normal, float shadow){
 	vec2 res=vec2(0.0,0.0);
 	if(shadow==0.0){return res;}
 	
 	vec3 incident = -normalize ( light.direction );
 	vec3 halfDir = normalize ( incident + viewDir );
 	float lambert = clamp (dot ( incident , normal ) ,0.0 ,1.0);
 	float specFactor = clamp (dot ( halfDir , normal ) ,0.0 ,1.0);
 	specFactor = pow ( specFactor , 60.0 );
 	res.x=lambert;
 	res.y=specFactor;

 	return res;
 }

 vec2 CalculatePointLight(PointLight light,vec3 viewDir,vec3 normal,vec3 worldPos){
 	vec2 res=vec2(0.0,0.0);
 	float dist = length ( light.position - worldPos );
    float atten = 1.0 - clamp ( dist / light.radius , 0.0 , 1.0);
    if( atten == 0.0) {
 		return res;
 	}

 	vec3 incident = normalize ( light.position - worldPos );
 	vec3 halfDir = normalize ( incident + viewDir );
 	float lambert = clamp (dot ( incident , normal ) ,0.0 ,1.0);
 	float specFactor = clamp (dot ( halfDir , normal ) ,0.0 ,1.0);
 	specFactor = pow ( specFactor , 32.0 );
 	res.x=lambert*atten;
 	res.y=specFactor*atten;
 	return res;

 }

 void main ( void ) {
 vec2 texCoord = vec2 ( gl_FragCoord .xy * pixelSize );//用了一个内置变量
 float depth = texture ( depthTex , texCoord .xy ).r;
 vec3 ndcPos = vec3 ( texCoord , depth ) * 2.0 - 1.0;
 vec4 invClipPos = inverseProjView * vec4 (ndcPos , 1.0);
 vec3 worldPos = invClipPos .xyz / invClipPos .w;//这里也要除一下w分量

//计算光照衰减
 //float dist = length ( lightPos - worldPos );
 //float atten = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

 //if( atten == 0.0) {
 //discard ;
 //}

 //vec3 normal = normalize ( texture ( normTex , texCoord .xy ). xyz *2.0 -1.0);
 //vec3 incident = normalize ( lightPos - worldPos );
 //vec3 viewDir = normalize ( cameraPos - worldPos );
 //vec3 halfDir = normalize ( incident + viewDir );

 //float lambert = clamp (dot ( incident , normal ) ,0.0 ,1.0);
 //float specFactor = clamp (dot ( halfDir , normal ) ,0.0 ,1.0);
 //specFactor = pow ( specFactor , 60.0 );
 //vec3 attenuated = lightColour .xyz * atten ;
 //diffuseOutput = vec4 ( attenuated * lambert , 1.0);
 //specularOutput = vec4 ( attenuated * specFactor * 0.33 , 1.0);

float shadow=texture ( shadowTex , texCoord .xy ).x;
vec3 normal = normalize ( texture ( normTex , texCoord .xy ). xyz *2.0 -1.0);
 vec3 viewDir = normalize ( cameraPos - worldPos );
 
 vec2 res=vec2(0.0,0.0);
 if(lightType==0){
 	res=CalculateDirectionalLight(directionalLight, viewDir, normal, shadow);
 	diffuseOutput = vec4 ( directionalLight.color.xyz * res.x, 1.0);
	specularOutput = vec4 ( directionalLight.color.xyz * res.y*0.33 , 1.0);
 }else if(lightType==1){
 	res=CalculatePointLight(pointLight,viewDir, normal,worldPos);
 	diffuseOutput = vec4 ( pointLight.color.xyz * res.x, 1.0);
	specularOutput = vec4 ( pointLight.color.xyz * res.y*0.33 , 1.0);
 }



 }