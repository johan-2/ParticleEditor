#include "QuadShader.h"
#include "DXManager.h"
#include "QuadComponent.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ShaderHelpers.h"
#include "DXBlendStates.h"
#include "DXDepthStencilStates.h"

QuadShader::QuadShader()
{
	// create quad shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSprite.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSprite.ps",   _pixelShader,  _pixelShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferPixel);
}

QuadShader::~QuadShader()
{
	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_vertexShader->Release();
	_pixelShader->Release();

	_constantBufferPixel->Release();
	_constantBufferVertex->Release();
}

void QuadShader::RenderQuadUI(const std::vector<QuadComponent*>& quads)
{
	// get DXManager
	DXManager& DXM = DXManager::GetInstance();

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set shaders			
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set constant buffers
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferPixel);

	// render with alpha blending
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_ALPHA);

	// render with depth off
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	
	// constant buffer structures
	ConstantQuadUIVertex vertexData;
	ConstantQuadUIPixel  pixelData;

	// fill constant buffer vertex with the camera matrices
	vertexData.projection = CameraManager::GetInstance().GetCurrentCameraUI()->GetViewMatrix();
	vertexData.view       = CameraManager::GetInstance().GetCurrentCameraUI()->GetProjectionMatrix();

	// update vertexconstantbuffers, only needs to be done once for all quads
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantQuadUIVertex), _constantBufferVertex);

	for (int i = 0; i < quads.size(); i++)
	{
		// set the color of this quad in the constant pixel buffer
		pixelData.color = quads[i]->GetColor();

		// update the buffer for each quad
		SHADER_HELPERS::UpdateConstantBuffer((void*)&pixelData, sizeof(ConstantQuadUIPixel), _constantBufferPixel);

		// get the quad texture
		ID3D11ShaderResourceView* texture = quads[i]->GetTexture();

		// set the texture
		devCon->PSSetShaderResources(0, 1, &texture);

		// set the vertex and inex buffers
		quads[i]->UploadBuffers();

		// draw
		devCon->DrawIndexed(6, 0, 0);
	}

	// enable depth after the UI Rendering is done
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);
}
