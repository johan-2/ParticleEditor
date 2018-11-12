#include "DXSamplerStates.h"
#include <iostream>

DXSamplerStates::DXSamplerStates(ID3D11Device* device, ID3D11DeviceContext* devCon):
	_device(device),
	_devCon(devCon)
{
	CreateSamplerStates();
}

DXSamplerStates::~DXSamplerStates()
{
	_sampleStateWrap->Release();
	_sampleStateClamp->Release();
}

void DXSamplerStates::CreateSamplerStates()
{
	HRESULT result;
	D3D11_SAMPLER_DESC samplerDesc;

	// create sampler state wrap
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = _device->CreateSamplerState(&samplerDesc, &_sampleStateWrap);
	if (FAILED(result))
		printf("failed to create samplestate wrap");

	// create sampler state clamp
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = _device->CreateSamplerState(&samplerDesc, &_sampleStateClamp);
	if (FAILED(result))
		printf("failed to create samplestate clamp");

	// set these samplers to be used in the pixelshaders
	ID3D11SamplerState* samplers[2]{ _sampleStateWrap, _sampleStateClamp };
	_devCon->PSSetSamplers(0, 2, samplers);
}
