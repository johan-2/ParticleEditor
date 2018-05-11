
uniform matrix u_worldMatrix;
uniform matrix u_viewMatrix;
uniform matrix u_projectionMatrix;
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
		
};


PixelInputType Main(VertexInputType input)
{
    PixelInputType output;   

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
   
    output.position = mul(input.position, u_worldMatrix); 	
    output.position = mul(output.position, u_viewMatrix); 
    output.position = mul(output.position, u_projectionMatrix);
    	   	
		   		
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex + u_uvOffset;
	
    
    return output;
}