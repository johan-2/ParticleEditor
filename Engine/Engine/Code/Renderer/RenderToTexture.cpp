#include "RenderToTexture.h"
#include "DXManager.h"
#include <iostream>
#include "Systems.h"
#include "DXErrorHandler.h"

RenderToTexture::RenderToTexture(unsigned int width, unsigned int height, bool depthOnly) 	
{
	ID3D11Device* device = Systems::dxManager->GetDevice();
	HRESULT result;

	// if we are only interessted in rendering to a new depth stencil view
	if (depthOnly)
	{
		// depth texture
		ID3D11Texture2D* depthTex2D;

		// setup the description for TEXTURE2D
		D3D11_TEXTURE2D_DESC depthStencilTexDesc;
		ZeroMemory(&depthStencilTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilTexDesc.Width = width;
		depthStencilTexDesc.Height = height;
		depthStencilTexDesc.MipLevels = 1;
		depthStencilTexDesc.ArraySize = 1;
		depthStencilTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilTexDesc.SampleDesc.Count = 1;
		depthStencilTexDesc.SampleDesc.Quality = 0;
		depthStencilTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilTexDesc.CPUAccessFlags = 0;
		depthStencilTexDesc.MiscFlags = 0;
	
		// setup description for depthstencilview 
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;
		
		// setup description for shaderresource
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		resourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MostDetailedMip = 0;
		resourceViewDesc.Texture2D.MipLevels = 1;

		// create the depth texture using the description
		result = device->CreateTexture2D(&depthStencilTexDesc, NULL, &depthTex2D);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create Texture2D in render texture");

		// create depthstencilview
		result = device->CreateDepthStencilView(depthTex2D, &depthStencilViewDesc, &_depthStencilView);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create depth stencil in render texture");

		result = device->CreateShaderResourceView(depthTex2D, &resourceViewDesc, &_shaderResoureView);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create SRV in render texture");

		_viewport.Width = (float)width;
		_viewport.Height = (float)height;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		depthTex2D->Release();		
	}
	else // create both a render target view and a depth stencil view
	{		
		ID3D11Texture2D* depthTex2D = nullptr;
		ID3D11Texture2D* renderTex2D = nullptr;

		// rendertarget texture description
		D3D11_TEXTURE2D_DESC RenderTargetTexDesc;
		ZeroMemory(&RenderTargetTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		RenderTargetTexDesc.Width = width;
		RenderTargetTexDesc.Height = height;
		RenderTargetTexDesc.MipLevels = 1;
		RenderTargetTexDesc.ArraySize = 1;
		RenderTargetTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		RenderTargetTexDesc.SampleDesc.Count = 1;
		RenderTargetTexDesc.Usage = D3D11_USAGE_DEFAULT;
		RenderTargetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		RenderTargetTexDesc.CPUAccessFlags = 0;
		RenderTargetTexDesc.MiscFlags = 0;

		// setup the description for depthtexture
		D3D11_TEXTURE2D_DESC depthStencilTexDesc;
		ZeroMemory(&depthStencilTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		depthStencilTexDesc.Width = width;
		depthStencilTexDesc.Height = height;
		depthStencilTexDesc.MipLevels = 1;
		depthStencilTexDesc.ArraySize = 1;
		depthStencilTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilTexDesc.SampleDesc.Count = 1;
		depthStencilTexDesc.SampleDesc.Quality = 0;
		depthStencilTexDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilTexDesc.CPUAccessFlags = 0;
		depthStencilTexDesc.MiscFlags = 0;

		// rendertargetview description
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		renderTargetViewDesc.Format = RenderTargetTexDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;		

		// setup description for depthstencilview 
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		depthStencilViewDesc.Format = depthStencilTexDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;

		// setup description for shaderresource
		D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
		ZeroMemory(&resourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		resourceViewDesc.Format = RenderTargetTexDesc.Format;
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MostDetailedMip = 0;
		resourceViewDesc.Texture2D.MipLevels = 1;

		result = device->CreateTexture2D(&RenderTargetTexDesc, NULL, &renderTex2D);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create Texture2D for depth in render texture");

		// create the depth texture using the description
		result = device->CreateTexture2D(&depthStencilTexDesc, NULL, &depthTex2D);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create Texture2D for render target in render texture");

		result = device->CreateRenderTargetView(renderTex2D, &renderTargetViewDesc, &_renderTargetView);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create render target in render texture");

		// create depthstencilview
		result = device->CreateDepthStencilView(depthTex2D, &depthStencilViewDesc, &_depthStencilView);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create depth stencil in render texture");

		result = device->CreateShaderResourceView(renderTex2D, &resourceViewDesc, &_shaderResoureView);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create SRV in render texture");

		_viewport.Width = (float)width;
		_viewport.Height = (float)height;
		_viewport.MinDepth = 0.0f;
		_viewport.MaxDepth = 1.0f;
		_viewport.TopLeftX = 0.0f;
		_viewport.TopLeftY = 0.0f;

		depthTex2D->Release();
		renderTex2D->Release();		
	}	
}

RenderToTexture::~RenderToTexture()
{
	if (_shaderResoureView)
		_shaderResoureView->Release();

	if (_textureView)
		_textureView->Release();

	if (_renderTargetView)
		_renderTargetView->Release();
}

void RenderToTexture::SetRendertarget()
{
	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();

	devCon->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	devCon->RSSetViewports(1, &_viewport);
}

void RenderToTexture::ClearRenderTarget(float r, float g, float b, float a, bool depthOnly)
{
	float color[4]{ r,g,b,a };

	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();

	if (depthOnly) 
	{
		devCon->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		return;
	}

	devCon->ClearRenderTargetView(_renderTargetView, color);
	devCon->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}