#include "ParticleShader.h"
#include "DXManager.h"
#include "DXDepthStencilStates.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ParticleSystemComponent.h"
#include "ShaderHelpers.h"
#include "Systems.h"

ParticleShader::ParticleShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexParticle.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelParticle.ps",   _pixelShader,  _pixelShaderByteCode);

	// create constant buffer for vertex shader
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
}

ParticleShader::~ParticleShader()
{
	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_vertexShader->Release();
	_pixelShader->Release();

	_constantBufferVertex->Release();
}

void ParticleShader::RenderParticles(const std::vector<ParticleSystemComponent*>& systems)
{
	if (systems.size() == 0)
		return;

	// get DX and camera manager
	DXManager& DXM    = *Systems::dxManager;
	CameraManager& CM = *Systems::cameraManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get the game camera
	CameraComponent* camera = CM.currentCameraGame;

	// set shaders			
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// constantbuffer structures
	CBVertex vertexData;

	// set the vertex constant data
	XMStoreFloat4x4(&vertexData.viewProj, XMLoadFloat4x4(&camera->viewProjMatrixTrans));
	
	// update the vertex constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBVertex), _constantBufferVertex);

	// don't write the particles to the depth buffer, only read
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::READ_ONLY);

	// loop over all particle systems
	for (int i = 0; i < systems.size(); i++)
	{
		// loop over all emitters in each system
		for (int y = 0; y < systems[i]->GetNumEmitters(); y++)
		{
			// upload vertex and indexbuffers
			systems[i]->UploadBuffers(y);

			// set the blend state that this emitter uses
			// TODO: this is probably going to set the same blend state
			// multiple times in a row, fix this
			DXM.BlendStates()->SetBlendState(systems[i]->GetBlendState(y));

			// get texture
			ID3D11ShaderResourceView* texture = systems[i]->GetTexture(y);

			// set texture
			devCon->PSSetShaderResources(0, 1, &texture);

			// draw	instanced with the same 6 indices and 4 vertices for all particles	
			devCon->DrawIndexedInstanced(6, systems[i]->GetNumParticles(y), 0, 0, 0);
		}
	}
	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[1] = { NULL};
	devCon->PSSetShaderResources(0, 1, nullSRV);
	// enable depth writing
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);
}
