#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "DXErrorHandler.h"

namespace SHADER_HELPERS
{
	static void UpdateConstantBuffer(void* data, unsigned int size, ID3D11Buffer*& buffer)
	{
		ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
		ID3D11Device* device = DXManager::GetInstance().GetDevice();

		//get desc of the current constant buffer in use
		D3D11_BUFFER_DESC old;
		buffer->GetDesc(&old);

		// if smaller then the new data we create a new bigger one and remove the old
		if (old.ByteWidth < size)
		{
			buffer->Release();
			buffer = nullptr;

			HRESULT result = 0;
			D3D11_BUFFER_DESC constVertexBufferDesc;

			constVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constVertexBufferDesc.ByteWidth = size;
			constVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constVertexBufferDesc.MiscFlags = 0;
			constVertexBufferDesc.StructureByteStride = 0;

			result = device->CreateBuffer(&constVertexBufferDesc, NULL, &buffer);
			if (FAILED(result))
				DX_ERROR::PrintError(result, "failed to create new constant buffer when upgrading size");
		}

		//uppdate the buffer with the new data
		void* destination;
		D3D11_MAPPED_SUBRESOURCE subResource;
		devCon->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);

		memcpy(subResource.pData, data, size);

		devCon->Unmap(buffer, 0);
	}

	static void CreateConstantBuffer(ID3D11Buffer*& buffer)
	{
		ID3D11Device* device = DXManager::GetInstance().GetDevice();

		D3D11_BUFFER_DESC constantBufferdesc;
		constantBufferdesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferdesc.ByteWidth = 16;
		constantBufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferdesc.MiscFlags = 0;
		constantBufferdesc.StructureByteStride = 0;

		HRESULT result;

		result = device->CreateBuffer(&constantBufferdesc, NULL, &buffer);
		if (FAILED(result))
			DX_ERROR::PrintError(result, "failed to create new constant buffer");
	}

	static void CreateVertexShader(LPCWSTR filePath, ID3D11VertexShader*& shader, ID3D10Blob*& buffer)
	{
		ID3D11Device* device = DXManager::GetInstance().GetDevice();
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
		ID3D11Device* device = DXManager::GetInstance().GetDevice();
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
}