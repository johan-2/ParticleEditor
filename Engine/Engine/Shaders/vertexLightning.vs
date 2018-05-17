
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
       	   	
	output.position = input.position;
    output.tex = input.tex;
	   
    return output;
}