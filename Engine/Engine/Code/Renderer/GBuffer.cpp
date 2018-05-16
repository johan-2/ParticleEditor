#include "GBuffer.h"
#include "Systemdefs.h"
#include "DXManager.h"
#include <iostream>


GBuffer::GBuffer()
{
	CreateRenderTargets();
}


GBuffer::~GBuffer()
{
}

void GBuffer::SetRenderTargets()
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
	DXManager& dXM = DXManager::GetInstance();
	ID3D11DepthStencilView*& depthStencil = DXManager::GetInstance().GetDeathStencilview();
	
	float clear[4] = { 0,0,0,1 };

	for(int i =0; i < 4; i++)
		devCon->ClearRenderTargetView(_renderTargetArray[i], clear);

	dXM.SetViewport(nullptr, true);
	devCon->OMSetRenderTargets(4, _renderTargetArray, depthStencil);
}

void GBuffer::CreateRenderTargets()
{
	_renderTargetArray = new ID3D11RenderTargetView*[4];
	_srvArray = new ID3D11ShaderResourceView*[4];
	_tex2DArray = new ID3D11Texture2D*[4];

	ID3D11Device* device = DXManager::GetInstance().GetDevice();

	HRESULT result;

	// rendertarget texture description
	D3D11_TEXTURE2D_DESC RenderTargetTexDesc;
	ZeroMemory(&RenderTargetTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
	RenderTargetTexDesc.Width = SCREEN_WIDTH;
	RenderTargetTexDesc.Height = SCREEN_HEIGHT;
	RenderTargetTexDesc.MipLevels = 1;
	RenderTargetTexDesc.ArraySize = 1;
	RenderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	RenderTargetTexDesc.SampleDesc.Count = 1;
	RenderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	RenderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	RenderTargetTexDesc.CPUAccessFlags = 0;
	RenderTargetTexDesc.MiscFlags = 0;

	// rendertargetview description
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.Format = RenderTargetTexDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// setup description for shaderresource
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	resourceViewDesc.Format = RenderTargetTexDesc.Format;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;
	resourceViewDesc.Texture2D.MipLevels = 1;	

	for(int i =0; i< 4; i++)
	{
		result = device->CreateTexture2D(&RenderTargetTexDesc, NULL, &_tex2DArray[i]);
		if (FAILED(result))
			printf("failed to create rendertexture2d in GBUFFER to texture\n");

		// position, normal, diffuse, specular
		result = device->CreateRenderTargetView(_tex2DArray[i], &renderTargetViewDesc, &_renderTargetArray[i]);
		if (FAILED(result))
			printf("failed to create rendertargetview in GBUFFER\n");

		result = device->CreateShaderResourceView(_tex2DArray[i], &resourceViewDesc, &_srvArray[i]);
		if (FAILED(result))
			printf("failed to create shaderResourceView in GBUFFER\n");

		_tex2DArray[i]->Release();
	}

	delete[] _tex2DArray;

}
