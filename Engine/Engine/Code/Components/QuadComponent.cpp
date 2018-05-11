#include "QuadComponent.h"
#include "DXManager.h"
#include "TexturePool.h"
#include "Entity.h"
#include "Renderer.h"
#include "SystemDefs.h"

QuadComponent::QuadComponent() : IComponent(COMPONENT_TYPE::QUAD_COMPONENT)
{			
	
}


QuadComponent::~QuadComponent()
{
	if (_indexBuffer)
		_indexBuffer->Release();

	if (_vertexBuffer)
		_vertexBuffer->Release();	

	Renderer::GetInstance().RemoveQuadFromUIRenderer(this);
}

void QuadComponent::Init(XMFLOAT2 position, XMFLOAT2 size, wchar_t* texturePath, XMFLOAT4 color)
{

	_size = size;
	_position = position;
	_color = color;
	_prevSize = size;
	_PrevPosition = position;

	CreateBuffers();
	_texture = TexturePool::GetInstance().GetTexture(texturePath);

	Renderer::GetInstance().AddQuadToUIRenderer(this);

}


void QuadComponent::Update()
{
	UpdateBuffers();
}

void QuadComponent::CreateBuffers()
{

	ID3D11Device* device = DXManager::GetInstance().GetDevice();
	VertexType* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	
	float left, right, top, bottom;
			
	vertices = new VertexType[4];		
	indices = new unsigned int[6]{ 0,1,2,3,1,0 };
	
	// Calculate the screen coordinates of the sprite, convert so 0.0 now is the top left corner instead of the middle
	// also convert so the pivot point is always in the middle of the sprite
	// possible uppgrade, make parameters for chosing where to have the pivot, could be good for ex left alignment
	left = (float)((SCREEN_WIDTH / 2) * -1) + _position.x -(_size.x /2);	
	right = left + _size.x;	
	top = (float)(SCREEN_HEIGHT / 2) - _position.y + (_size.y /2);	
	bottom = top - _size.y;

	// set data of vertices
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);	
	vertices[3].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[3].texture = XMFLOAT2(1.0f, 0.0f);
		
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

void QuadComponent::UpdateBuffers()
{

	unsigned int result = XMVector4EqualR(XMLoadFloat2(&_PrevPosition), XMLoadFloat2(&_position));
	unsigned int result2 = XMVector4EqualR(XMLoadFloat2(&_prevSize), XMLoadFloat2(&_size));

		// only uppdate buffer if the pos or scale have changed
	if (XMComparisonAnyFalse(result) || XMComparisonAnyFalse(result2))
	{
		_PrevPosition = _position;
		_prevSize = _size;
		
		ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();
		D3D11_MAPPED_SUBRESOURCE vertexData;
		HRESULT result;
		VertexType* vertices = new VertexType[4];				

		// calculate all corners of quad
		float left, right, top, bottom;
		left = (float)((SCREEN_WIDTH / 2) * -1) + _position.x - (_size.x / 2);
		right = left + _size.x;
		top = (float)(SCREEN_HEIGHT / 2) - _position.y + (_size.y / 2);
		bottom = top - _size.y;
		
		// set the data of vertices
		vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
		vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
		vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
		vertices[1].texture = XMFLOAT2(1.0f, 1.0f);
		vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
		vertices[2].texture = XMFLOAT2(0.0f, 1.0f);		
		vertices[3].position = XMFLOAT3(right, top, 0.0f);  // Top right.
		vertices[3].texture = XMFLOAT2(1.0f, 0.0f);

		// map the vertex buffer 		
		result = devCon->Map(_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexData);
		if (FAILED(result))
			printf("Failed to map spriteVertexBuffer");

		// copy the new vertices positions into the buffer 		
		memcpy(vertexData.pData, (void*)vertices, sizeof(VertexType) * 4);

		//unmap buffer
		devCon->Unmap(_vertexBuffer, 0);										
																				 
		delete[]vertices;				
	}
}

void QuadComponent::UploadBuffers()
{
	ID3D11DeviceContext* devCon = DXManager::GetInstance().GetDeviceCon();

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	

}


