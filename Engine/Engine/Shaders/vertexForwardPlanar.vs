uniform float4x4 u_worldMatrix;
uniform float4x4 u_viewMatrix;
uniform float4x4 u_projectionMatrix;
uniform float4x4 u_lightViewMatrix;
uniform float4x4 u_lightProjectionMatrix;
uniform float4x4 u_reflectionViewMatrix;
uniform float3 	 u_cameraPosition;
uniform float2   u_uvOffset;

struct VertexInputType
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float4 color    : COLOR;
};

struct PixelInputType
{
    float4 position           : SV_POSITION;
    float2 texCoord           : TEXCOORD0;
	float3 normal             : NORMAL;
	float3 tangent            : TANGENT;
	float3 binormal           : BINORMAL;
	float3 vertexToCamera     : TEXCOORD1;
	float4 positionLightSpace : TEXCOORD2;	 
	float3 worldPos           : TEXCOORD3;
	float4 vertexColor        : COLOR;
	float4 reflectionPosition : TEXCOORD4;
};

PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
	
	// send texcoords to pixelshader
	output.texCoord = input.texCoord + u_uvOffset;
	  
    // set the w component of position
    input.position.w = 1.0f;
	
	// get position in the worldViewProjection
    output.position = mul(input.position,  u_worldMatrix); 	
    output.position = mul(output.position, u_viewMatrix); 
    output.position = mul(output.position, u_projectionMatrix);
    	   	
	// transform normals to worldSpace
	output.normal   = normalize(mul(input.normal,   (float3x3)u_worldMatrix));	     	
	output.tangent  = normalize(mul(input.tangent,  (float3x3)u_worldMatrix));		
	output.binormal = normalize(mul(input.binormal, (float3x3)u_worldMatrix));
	
	// transform position into the space of the directional light that will calculate the shadows
	output.positionLightSpace = mul(input.position,            u_worldMatrix); 	
    output.positionLightSpace = mul(output.positionLightSpace, u_lightViewMatrix); 
    output.positionLightSpace = mul(output.positionLightSpace, u_lightProjectionMatrix);
					
	// get the direction from vertex to camera for specular calculations	
	float4 worldPosition  = mul(input.position, u_worldMatrix);		
	output.vertexToCamera = u_cameraPosition.xyz - worldPosition.xyz;
	
	// send world pos to pixel shader for point light calculations
	output.worldPos = worldPosition;
	
	// get the reflection worldview matrix
	matrix reflectWorldViewMatrix = mul(u_worldMatrix ,mul(u_reflectionViewMatrix, u_projectionMatrix));
	
	// get reflection position
	output.reflectionPosition = mul(input.position, reflectWorldViewMatrix); 	
    
	// vertex color
	output.vertexColor = input.color;
		   
    return output;
}













