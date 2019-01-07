#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "DXManager.h"
#include "DXErrorHandler.h"
#include <vector>
#include "Mesh.h"
#include "Systems.h"

namespace SHADER_HELPERS
{
	static void UpdateConstantBuffer(void* data, unsigned int size, ID3D11Buffer*& buffer)
	{
		ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();
		ID3D11Device* device        = Systems::dxManager->GetDevice();

		//get desc of the current constant buffer in use
		D3D11_BUFFER_DESC old;
		buffer->GetDesc(&old);

		// if smaller then the new data we create a new bigger one and remove the old
		if (old.ByteWidth < size)
		{
			buffer->Release();
			buffer = nullptr;

			HRESULT result = 0;
			D3D11_BUFFER_DESC bufferDesc;

			bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth           = size;
			bufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags           = 0;
			bufferDesc.StructureByteStride = 0;

			result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
			if (FAILED(result))
				DX_ERROR::PrintError(result, "failed to create new constant buffer when upgrading size");
		}

		// uppdate the buffer with the new data
		D3D11_MAPPED_SUBRESOURCE subResource;
		devCon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

		memcpy(subResource.pData, data, size);

		devCon->Unmap(buffer, 0);
	}

	static void CreateConstantBuffer(ID3D11Buffer*& buffer)
	{
		ID3D11Device* device = Systems::dxManager->GetDevice();

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth           = 16;
		bufferDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags           = 0;
		bufferDesc.StructureByteStride = 0;

		HRESULT result;

		result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create new constant buffer");
	}

	static void CreateStructuredBuffer(void* data, unsigned int structSize, unsigned int count, ID3D11Buffer*& buffer, ID3D11ShaderResourceView*& bufferSRV)
	{
		ID3D11Device* device = Systems::dxManager->GetDevice();

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth           = structSize * count;
		bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = structSize;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format              = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements  = count;

		HRESULT result;

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = data;

		result = device->CreateBuffer(&bufferDesc, &subResource, &buffer);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create new structured buffer");

		result = device->CreateShaderResourceView(buffer, &srvDesc, &bufferSRV);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create SRV for structured buffer");
	}

	static void UpdateStructuredBuffer(void* data, unsigned int structSize, unsigned int count, ID3D11Buffer*& buffer)
	{
		ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();
		ID3D11Device* device = Systems::dxManager->GetDevice();

		//get desc of the current constant buffer in use
		D3D11_BUFFER_DESC old;
		buffer->GetDesc(&old);

		// if smaller then the new data we create a new bigger one and remove the old
		if (old.ByteWidth < (structSize * count))
		{
			buffer->Release();
			buffer = nullptr;

			HRESULT result = 0;
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth           = structSize * count;
			bufferDesc.BindFlags           = D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = structSize;

			result = device->CreateBuffer(&bufferDesc, NULL, &buffer);
			if (FAILED(result))
				DX_ERROR::PrintError(result, "failed to create new structured buffer while upgrading size");
		}

		// uppdate the buffer with the new data
		D3D11_MAPPED_SUBRESOURCE subResource;
		devCon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

		memcpy(subResource.pData, data, (structSize * count));

		devCon->Unmap(buffer, 0);
	}

	static void CreateVertexShader(LPCWSTR filePath, ID3D11VertexShader*& shader, ID3D10Blob*& buffer)
	{
		ID3D11Device* device = Systems::dxManager->GetDevice();
		HRESULT result;
		ID3D10Blob* errorMessage;

		// compile the vertex shader code from the text file into temporal buffers
		result = D3DCompileFromFile(filePath, NULL, NULL, "Main", "vs_5_0", 0, 0, &buffer, &errorMessage);
		if (FAILED(result))
		{
			DX_ERROR::PrintError(result, (std::string("failed to compile Vertex shader : ") + DX_ERROR::ConvertFromWString((wchar_t*)filePath)).c_str());

			if (errorMessage)			
				printf("%s", (char*)errorMessage->GetBufferPointer());		
		}

		// once the code have been compiled into the buffer we can create the shader objects themselfs from the blob objects
		result = device->CreateVertexShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
			DX_ERROR::PrintError(result, (std::string("failed to create vertex shader from ID3D10Blob, shader file = : ") + DX_ERROR::ConvertFromWString((wchar_t*)filePath)).c_str());
	}

	static void CreatePixelShader(LPCWSTR filePath, ID3D11PixelShader*& shader, ID3D10Blob*& buffer)
	{
		ID3D11Device* device = Systems::dxManager->GetDevice();
		HRESULT result;
		ID3D10Blob* errorMessage;

		// compile the pixel shader code from the text file into temporal buffers
		result = D3DCompileFromFile(filePath, NULL, NULL, "Main", "ps_5_0", 0, 0, &buffer, &errorMessage);
		if (FAILED(result))
		{
			DX_ERROR::PrintError(result, (std::string("failed to compile Pixel shader : ") + DX_ERROR::ConvertFromWString((wchar_t*)filePath)).c_str());

			if (errorMessage)			
				printf("%s", (char*)errorMessage->GetBufferPointer());			
		}

		//once the code have been compiled into the buffer we can create the shader objects themselfs from the blob objects
		result = device->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &shader);
		if (FAILED(result))
			DX_ERROR::PrintError(result, (std::string("failed to create pixel shader from ID3D10Blob, shader file = : ") + DX_ERROR::ConvertFromWString((wchar_t*)filePath)).c_str());
	}

	static void MeshSort(std::vector<Mesh*>& meshes, const XMFLOAT3& position, bool backToFront)
	{
		const int size = (int)meshes.size();

		for (int i = 0; i < size; i++)
		{
			XMFLOAT3 vec;
			float distance = 0;

			XMStoreFloat3(&vec, XMVectorSubtract(XMLoadFloat3(&meshes[i]->GetPosition()), XMLoadFloat3(&position)));
			XMStoreFloat(&distance, XMVector3Length(XMLoadFloat3(&vec)));

			meshes[i]->SetDistanceToCamera(distance);
		}

		if (backToFront)
			std::sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b) -> bool {return a->GetDistanceFromCamera() > b->GetDistanceFromCamera(); });
		else
			std::sort(meshes.begin(), meshes.end(), [](Mesh* a, Mesh* b) -> bool {return a->GetDistanceFromCamera() < b->GetDistanceFromCamera(); });
	}
}