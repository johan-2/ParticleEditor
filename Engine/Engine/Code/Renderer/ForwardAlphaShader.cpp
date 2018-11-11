#include "ForwardAlphaShader.h"
#include "ShaderHelpers.h"

ForwardAlphaShader::ForwardAlphaShader()
{
	//create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexForward.vs",  _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelForward.ps",    _pixelShader,  _pixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPixelAmbDir);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPixelPoint);
}

ForwardAlphaShader::~ForwardAlphaShader()
{
}

void ForwardAlphaShader::RenderForward(std::vector<Mesh*>& meshes)
{

}