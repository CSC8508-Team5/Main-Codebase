# version 330 core

 uniform sampler2D diffuseTex ; 
 uniform sampler2D shadowTex;
 uniform vec3 lightDir;
 uniform bool hasTexture;

 in Vertex {
	vec2 texCoord ;
 	vec3 normal ;
 	vec4 shadowProj;
    vec4 colour;
 } IN;

 layout (location = 0) out vec4 diffTex;
  layout (location = 1) out vec4 normTex;
  layout (location = 2) out vec4 shadowOutTex;

 float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowTex, projCoords.xy).r;
    //return closestDepth;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowTex, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowTex, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth  > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}



 void main ( void ) {
 float shadow=1.0-ShadowCalculation(IN.shadowProj,normalize(IN.normal),-normalize(lightDir));

 
vec4 albedo = IN.colour;
    
    if(hasTexture) {
     albedo *= texture(diffuseTex, IN.texCoord);
    }

diffTex =albedo;
 normTex = vec4 ( IN.normal .xyz * 0.5 + 0.5 ,1.0);
 shadowOutTex=vec4 ( shadow,0.0,0.0 ,1.0);
 }