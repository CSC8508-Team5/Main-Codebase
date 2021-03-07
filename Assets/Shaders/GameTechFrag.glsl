#version 330 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
//uniform sampler2DShadow shadowTex;
uniform sampler2D shadowTex;

struct DirectionalLight{
    vec3 direction;
    float ambient;
    vec4 color;
};

uniform DirectionalLight directionalLight;

uniform vec3	cameraPos;

uniform bool hasTexture;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowTex, projCoords.xy).r;
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


void main(void)
{
	//float shadow = 1.0; // New !
	
	//if( IN . shadowProj . w > 0.0) { // New !
		//shadow = textureProj ( shadowTex , IN . shadowProj ) * 0.5f;
	//}

	
	vec3  incident = normalize ( directionalLight.direction );
	//vec3  incident = normalize ( lightPos - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , IN.normal )) * 0.9; 

	float shadow=1.0-ShadowCalculation(IN.shadowProj,normalize(IN.normal),incident);
	
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	float rFactor = max (0.0 , dot ( halfDir , IN.normal ));
	float sFactor = pow ( rFactor , 80.0 );
	
	vec4 albedo = IN.colour;
	
	if(hasTexture) {
	 albedo *= texture(mainTex, IN.texCoord);
	}
	
	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * directionalLight.ambient; //ambient
	
	fragColor.rgb += albedo.rgb * directionalLight.color.rgb * lambert * shadow; //diffuse light
	
	fragColor.rgb += directionalLight.color.rgb * sFactor * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = albedo.a;

//fragColor.rgb = IN.normal;

	//fragColor = IN.colour;
	
	//fragColor.xy = IN.texCoord.xy;
	
	//fragColor = IN.colour;
}