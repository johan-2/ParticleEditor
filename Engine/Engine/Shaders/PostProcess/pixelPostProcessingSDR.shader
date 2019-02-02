Texture2D    Texture[1];    // ScenemapSDR
SamplerState SampleType[6]; // wrapTrilinear, clampTrilinear, wrapBilinear, clampBililinear, wrapAnisotropic, clampAnisotropic

uniform int u_applyFXAA;

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;		
};

float3 RGBToLuma(float3 rgb)
{
	return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}

float4 GetFXAAColor(PixelInputType input, float4 baseColor)
{
	return baseColor;
}

float4 Main(PixelInputType input) : SV_TARGET
{          	
	float4 baseColor = Texture[0].Load(int3(input.position.xy,0));
	
	if (u_applyFXAA == 1) baseColor = GetFXAAColor(input, baseColor);
	
	return baseColor;
}






























