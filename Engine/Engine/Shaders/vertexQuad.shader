uniform float4x4 u_viewProj;

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex      : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex      : TEXCOORD0;
};

PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
       
    input.position.w = 1.0f; 
  
    output.position = mul(input.position, u_viewProj); 
    output.tex = input.tex;	
    
    return output;
}