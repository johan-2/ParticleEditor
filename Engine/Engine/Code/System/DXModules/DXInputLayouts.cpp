#include "DXInputLayouts.h"
#include "DXManager.h"
#include <iostream>
#include "GuiManager.h"
#include "Systems.h"

DXInputLayouts::DXInputLayouts()
{
}

DXInputLayouts::~DXInputLayouts()
{
}

void DXInputLayouts::CreateInputLayout3D(ID3D10Blob*& vertexShaderByteCode)
{
	DXManager& DXM = *Systems::dxManager;

	ID3D11Device* device = DXM.GetDevice();

	D3D11_INPUT_ELEMENT_DESC inputLayout3D[6]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HRESULT result;
	result = device->CreateInputLayout(inputLayout3D, 6, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &_inputLayout3D);
	if (FAILED(result))
		printf("failed to create Inputlayout\n");
}

void DXInputLayouts::CreateInputLayout2D(ID3D10Blob*& vertexShaderByteCode)
{
	DXManager& DXM = *Systems::dxManager;

	ID3D11Device* device = DXM.GetDevice();

	D3D11_INPUT_ELEMENT_DESC inputLayout2D[2]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT result;
	result = device->CreateInputLayout(inputLayout2D, 2, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &_inputLayout2D);
	if (FAILED(result))
		printf("failed to create Inputlayout\n");
}

void DXInputLayouts::CreateInputLayoutParticle(ID3D10Blob*& vertexShaderByteCode)
{
	DXManager& DXM = *Systems::dxManager;

	ID3D11Device* device = DXM.GetDevice();

	D3D11_INPUT_ELEMENT_DESC inputLayoutParticle[8]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,                            0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,                            0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32_FLOAT,       1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	HRESULT result;
	result = device->CreateInputLayout(inputLayoutParticle, 8, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &_inputLayoutParticle);
	if (FAILED(result))
		printf("failed to create Inputlayout\n");
}

void DXInputLayouts::CreateInputLayoutGUI(ID3D10Blob*& vertexShaderByteCode)
{
	DXManager& DXM = *Systems::dxManager;

	ID3D11Device* device = DXM.GetDevice();

	D3D11_INPUT_ELEMENT_DESC inputLayoutGui[3]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT result;
	result = device->CreateInputLayout(inputLayoutGui, 3, vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &_inputLayoutGUI);
	if (FAILED(result))
		printf("failed to create Inputlayout\n");
}

void DXInputLayouts::SetInputLayout(INPUT_LAYOUT_TYPE type)
{
	DXManager& DXM = *Systems::dxManager;

	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();
	switch (type)
	{
	case LAYOUT3D:
		devCon->IASetInputLayout(_inputLayout3D);
		break;
	case LAYOUT2D:
		devCon->IASetInputLayout(_inputLayout2D);
		break;
	case LAYOUTPARTICLE:
		devCon->IASetInputLayout(_inputLayoutParticle);
		break;
	case LAYOUTGUI:
		devCon->IASetInputLayout(_inputLayoutGUI);
		break;
	default:
		break;
	}
}
