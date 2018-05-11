
uniform float4x4 u_viewMatrix;
uniform float4x4 u_projectionMatrix;


struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
		
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
		
};


// the vertexshader itself, takes a vertextype as input and returns a vector4


PixelInputType Main(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f; 

    //output.position = mul(input.position, u_worldMatrix); // takes the position of our vertices and multiply with the worldmatrix
    output.position = mul(input.position, u_viewMatrix); // next step we take the result from the last step and multiply with the viewmatrix
    output.position = mul(output.position, u_projectionMatrix); // last step is to transform the viewmatrix to our 2d screen space 
	
	
		   		
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
	
    
    return output;
}