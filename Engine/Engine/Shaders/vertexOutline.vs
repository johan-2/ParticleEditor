
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
	float3 viewDirection : TEXCOORD1;
};



PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
	    
    input.position.w = 1.0f;	   

    float4 original = mul(mul(mul(input.position,u_worldMatrix),u_viewMatrix),u_projectionMatrix);
	float4 normall = mul(mul(mul(input.normal,u_worldMatrix),u_viewMatrix),u_projectionMatrix);   		

	normall = normalize(normall);
	output.position = original + (mul(0.045f,normall));
	
   
    
    return output;
}