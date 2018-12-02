uniform matrix u_worldMatrix;
uniform matrix u_viewMatrix;
uniform matrix u_projectionMatrix;
uniform float4 u_clippingPlane;

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float4 color    : COLOR;	
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;	
	float clip      : SV_ClipDistance0;	
};

PixelInputType Main(VertexInputType input)
{
    PixelInputType output;   

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;
   
    // get the worldviewproj and multiply with position
    output.position = mul(input.position,  u_worldMatrix); 	
    output.position = mul(output.position, u_viewMatrix); 
    output.position = mul(output.position, u_projectionMatrix);
    	   			   		
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	
	// create the clipping plane to clip pixels under the reflection surface
	output.clip = dot(mul(input.position, u_worldMatrix), u_clippingPlane);
	  
    return output;
}