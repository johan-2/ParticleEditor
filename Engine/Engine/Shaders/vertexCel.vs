

uniform float4x4 u_worldMatrix;
uniform float4x4 u_viewMatrix;
uniform float4x4 u_projectionMatrix;


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	
};



PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
    
	
    input.position.w = 1.0f;
	
    // Calculate the position of the vertex against the world, view, and projection matrices.
	
    output.position = mul(input.position, u_worldMatrix);
    output.position = mul(output.position, u_viewMatrix);
    output.position = mul(output.position, u_projectionMatrix); 
    
	
    output.tex = input.tex;	
	output.normal = mul(input.normal, (float3x3)u_worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
		   
    
    return output;
}