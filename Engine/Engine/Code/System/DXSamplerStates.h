#pragma once
#include <D3D11.h>

class DXSamplerStates
{
public:
	DXSamplerStates(ID3D11Device* device, ID3D11DeviceContext* devCon);
	~DXSamplerStates();

private:

	// creates the sampler states
	void CreateSamplerStates();

	// pointers to the directx interfaces
	ID3D11Device*        _device;
	ID3D11DeviceContext* _devCon;

	// the sampler states
	ID3D11SamplerState* _sampleStateWrap;
	ID3D11SamplerState* _sampleStateClamp;
};

