#include "QuadComponent.h"
#include "DXManager.h"
#include "TexturePool.h"
#include "Entity.h"
#include "Renderer.h"
#include "SystemDefs.h"
#include "Systems.h"

QuadComponent::QuadComponent() : IComponent(COMPONENT_TYPE::QUAD_COMPONENT)
{				
}

QuadComponent::~QuadComponent()
{	
	_indexBuffer->Release();	
	_vertexBuffer->Release();	

	Systems::renderer->RemoveQuadFromRenderer(this);
}

void QuadComponent::Init(XMFLOAT2 position, XMFLOAT2 size, wchar_t* texturePath, XMFLOAT4 color, bool ignoreAlpha)
{
	this->size         = size;
	this->position     = position;
	this->color        = color;
	_prevSize          = size;
	_PrevPosition      = position;
	this->ignoreAlpha  = ignoreAlpha;

	CreateBuffers();
	texture = Systems::texturePool->GetTexture(texturePath);

	Systems::renderer->AddQuadToRenderer(this);
}

void QuadComponent::Update(const float& delta)
{
	UpdateBuffers();
}

void QuadComponent::CreateBuffers()
{
	// get device
	ID3D11Device*& device = Systems::dxManager->device;

	// create buffer descriptions and sub resource pointers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	
	float left, right, top, bottom;
			
	VertexType   vertices[4];
	unsigned int indices[6]{ 0,1,2,3,1,0 };
	
	// Calculate the screen coordinates of the quad, convert so 0.0 now is the top left corner of screen
	// also convert so the pivot point is always in the middle of the quad
	left   = (float)((SystemSettings::SCREEN_WIDTH / 2) * -1) + position.x -(size.x /2);
	right  = left + size.x;	
	top    = (float)(SystemSettings::SCREEN_HEIGHT / 2) - position.y + (size.y /2);
	bottom = top - size.y;
	
	// set data of vertices
	vertices[0].position = XMFLOAT3(left, top, 0.0f);      // Top left.
	vertices[0].texture  = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture  = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);   // Bottom left.
	vertices[2].texture  = XMFLOAT2(0.0f, 1.0f);	
	vertices[3].position = XMFLOAT3(right, top, 0.0f);     // Top right.
	vertices[3].texture  = XMFLOAT2(1.0f, 0.0f);
		
	// set vertexbuffer to be dynamic so we can update the data
	vertexBufferDesc.Usage               = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth           = sizeof(VertexType) * 4;
	vertexBufferDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags           = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// index buffer can be static, will always represent two triangles
	indexBufferDesc.Usage               = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth           = sizeof(unsigned int) * 6;
	indexBufferDesc.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags      = 0;
	indexBufferDesc.MiscFlags           = 0;
	indexBufferDesc.StructureByteStride = 0;

	// give pointer to vertex data
	vertexData.pSysMem          = vertices;
	vertexData.SysMemPitch      = 0;
	vertexData.SysMemSlicePitch = 0;

	// give pointer to index data
	indexData.pSysMem          = indices;
	indexData.SysMemPitch      = 0;
	indexData.SysMemSlicePitch = 0;

	// create buffers
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
		printf("failed to create vertexbuffer for quad\n");
					
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
		printf("failed to create vertexbuffer for quad\n");
}

void QuadComponent::UpdateBuffers()
{

	// get if position or scale have changed
	bool result  = XMVector4Equal(XMLoadFloat2(&_PrevPosition), XMLoadFloat2(&position));
	bool result2 = XMVector4Equal(XMLoadFloat2(&_prevSize), XMLoadFloat2(&size));	

	// only uppdate buffer if the pos or scale have changed
	if (!result || !result2)
	{
		// save last pos for next frame
		_PrevPosition = position;
		_prevSize     = size;
		
		// get device context
		ID3D11DeviceContext*& devCon = Systems::dxManager->devCon;

		// create resource pointer and vertex array
		D3D11_MAPPED_SUBRESOURCE vertexData;
		VertexType vertices[4];		

		HRESULT result;

		// calculate all corners of quad
		float left, right, top, bottom;
		left = (float)((SystemSettings::SCREEN_WIDTH / 2) * -1) + position.x - (size.x / 2);
		right = left + size.x;
		top = (float)(SystemSettings::SCREEN_HEIGHT / 2) - position.y + (size.y / 2);
		bottom = top - size.y;
				
		// set the data of vertices
		vertices[0].position = XMFLOAT3(left, top, 0.0f);      // Top left.
		vertices[0].texture  = XMFLOAT2(0.0f, 0.0f);
		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture  = XMFLOAT2(1.0f, 1.0f);
		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);   // Bottom left.
		vertices[2].texture  = XMFLOAT2(0.0f, 1.0f);		
		vertices[3].position = XMFLOAT3(right, top, 0.0f);     // Top right.
		vertices[3].texture  = XMFLOAT2(1.0f, 0.0f);

		// map the vertex buffer 		
		result = devCon->Map(_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexData);
		if (FAILED(result))
			printf("Failed to map spriteVertexBuffer");

		// copy the new vertices positions into the buffer 		
		memcpy(vertexData.pData, (void*)vertices, sizeof(VertexType) * 4);

		//unmap buffer
		devCon->Unmap(_vertexBuffer, 0);																														 		
	}
}

void QuadComponent::UploadBuffers()
{
	ID3D11DeviceContext*& devCon = Systems::dxManager->devCon;

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}


