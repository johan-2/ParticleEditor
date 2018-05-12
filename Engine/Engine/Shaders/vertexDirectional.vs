
uniform float4x4 u_worldMatrix;
uniform float4x4 u_viewMatrix;
uniform float4x4 u_projectionMatrix;
uniform float3 	 u_cameraPosition;
uniform float2   u_uvOffset;

struct VertexInputType
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 vertexToCamera : TEXCOORD1;
	 
};



PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
	
	// send texcoords to pixelshader
	output.texCoord = input.texCoord + u_uvOffset;
	  
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
	
	// get position acording to worldViewProjection
    output.position = mul(input.position, u_worldMatrix); 	
    output.position = mul(output.position, u_viewMatrix); 
    output.position = mul(output.position, u_projectionMatrix);
    	   	
	// transform normals to worldSpace
	output.normal = normalize(mul(input.normal, (float3x3)u_worldMatrix));	     	
	output.tangent = normalize(mul(input.tangent, (float3x3)u_worldMatrix));		
	output.binormal = normalize(mul(input.binormal, (float3x3)u_worldMatrix));
	
	
			   
	// get the direction from vertex to camera for specular calculations	
	float4 worldPosition = mul(input.position, u_worldMatrix);		
	output.vertexToCamera = normalize(u_cameraPosition.xyz - worldPosition.xyz);
	
	
	   
    return output;
}