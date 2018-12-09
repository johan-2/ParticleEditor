#include "PostProcessingShader.h"
#include "DXManager.h"
#include "Systems.h"
#include "ShaderHelpers.h"
#include "ScreenQuad.h"
#include "DXDepthStencilStates.h"
#include "DXBlendStates.h"

PostProcessingShader::PostProcessingShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexPostProcessing.vs", _vertexPostProcessingShader, _vertexPostProcessingShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelPostProcessing.ps",   _pixelPostProcessingShader,  _pixelPostProcessingShaderByteCode);
}

PostProcessingShader::~PostProcessingShader()
{
	_vertexPostProcessingShaderByteCode->Release();
	_pixelPostProcessingShaderByteCode->Release();

	_vertexPostProcessingShader->Release();
	_pixelPostProcessingShader->Release();
}

void PostProcessingShader::Render(ScreenQuad* quad, ID3D11ShaderResourceView* SRV)
{
	// get dx manager
	DXManager& DXM   = *Systems::dxManager;
	CameraManager CM = *Systems::cameraManager;

	// set to defult backbuffer and render our final scene image
	DXM.SetBackBuffer();

	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set our shaders
	devCon->VSSetShader(_vertexPostProcessingShader, NULL, 0);
	devCon->PSSetShader(_pixelPostProcessingShader, NULL, 0);

	quad->UploadBuffers();

	// set textures
	devCon->PSSetShaderResources(0, 1, &SRV);

	// draw
	devCon->DrawIndexed(6, 0, 0);	
}
