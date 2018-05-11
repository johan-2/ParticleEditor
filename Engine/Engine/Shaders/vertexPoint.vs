
uniform matrix u_worldMatrix;
uniform matrix u_viewMatrix;
uniform matrix u_projectionMatrix;

uniform float3 u_cameraPosition;
uniform float2 u_uvOffset;

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 vertexToCamera : TEXCOORD1;
	
	float3 worldPos : TEXCOORD2;
	
};


PixelInputType Main(VertexInputType input)
{
    PixelInputType output;   

    // send texcoords to pixelshader
	output.tex = input.tex + u_uvOffset;
	  
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
	
	output.worldPos = worldPosition.xyz;
    
    return output;
}

















