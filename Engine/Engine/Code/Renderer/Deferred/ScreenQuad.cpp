#include "ScreenQuad.h"
#include "DXManager.h"
#include <iostream>
#include "Systems.h"

ScreenQuad::ScreenQuad()
{
	CreateBuffers();
}

ScreenQuad::~ScreenQuad()
{
	_vertexBuffer->Release();
	_indexBuffer->Release();
}

void ScreenQuad::CreateBuffers()
{
	ID3D11Device* device = Systems::dxManager->GetDevice();
	VertexType* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	vertices = new VertexType[4];
	indices = new unsigned int[6]{ 0,1,2,2,1,3 };

	// set data of vertices
	vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // top right.
	vertices[1].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	// set vertexbuffer to be dynamic so we can update the data
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * 4;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// index buffer can be static, will always represent two triangles
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * 6;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// give pointer to vertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// give pointer to index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// create buffers
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
		printf("failed to create vertexbuffer for quad\n");

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
		printf("failed to create vertexbuffer for quad\n");

	// delete data after buffers been created
	delete[] vertices;
	delete[] indices;
}

void ScreenQuad::UploadBuffers()
{
	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

}

