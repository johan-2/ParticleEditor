#include "SkyBox.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "TransformComponent.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include "DXErrorHandler.h"
#include "ShaderHelpers.h"
#include "DXBlendstates.h"
#include "DXRasterizerStates.h"
#include "DXDepthStencilStates.h"
#include "Systems.h"

SkyBox::SkyBox(wchar_t* textureFile)
{
	// create mesh and cubemap
	CreateBox();
	LoadCubemap(textureFile);

	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/vertexSkyBox.vs", _vertexShader, _vertexShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/pixelSkyBox.ps", _pixelShader, _pixelShaderByteCode);

	// create constant buffer
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
}

SkyBox::~SkyBox()
{
	_vertexShaderByteCode->Release();
	_pixelShaderByteCode->Release();

	_vertexShader->Release();
	_pixelShader->Release();

	_vertexBuffer->Release();
	_indexBuffer->Release();

	_constantBufferVertex->Release();
}

void SkyBox::CreateBox() 
{
	// create vertex and index buffers
	VertexData    vertices[24];
	unsigned long indices[36]
	{
		0,1,2,2,1,3,
		4,5,6,6,5,7,
		8,9,10,10,9,11,
		12,13,14,14,13,15,
		16,17,18,18,17,19,
		20,21,22,22,21,23
	};

	vertices[0].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[0].texture  = XMFLOAT2(0, 0);
	vertices[0].normal   = XMFLOAT3(0, 0, -1);
	vertices[0].tangent  = XMFLOAT3(1, 0, 0);
	vertices[0].binormal = XMFLOAT3(-0, 1, 0);

	vertices[1].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[1].texture  = XMFLOAT2(1, 0);
	vertices[1].normal   = XMFLOAT3(0, 0, -1);
	vertices[1].tangent  = XMFLOAT3(1, 0, 0);
	vertices[1].binormal = XMFLOAT3(-0, 1, 0);

	vertices[2].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[2].texture  = XMFLOAT2(0, 1);
	vertices[2].normal   = XMFLOAT3(0, 0, -1);
	vertices[2].tangent  = XMFLOAT3(1, 0, 0);
	vertices[2].binormal = XMFLOAT3(-0, 1, 0);

	vertices[3].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[3].texture  = XMFLOAT2(1, 1);
	vertices[3].normal   = XMFLOAT3(0, 0, -1);
	vertices[3].tangent  = XMFLOAT3(1, 0, 0);
	vertices[3].binormal = XMFLOAT3(-0, 1, 0);

	vertices[4].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[4].texture  = XMFLOAT2(0, 0);
	vertices[4].normal   = XMFLOAT3(1, 0, 0);
	vertices[4].tangent  = XMFLOAT3(0, 0, 1);
	vertices[4].binormal = XMFLOAT3(0, 1, 0);

	vertices[5].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[5].texture  = XMFLOAT2(1, 0);
	vertices[5].normal   = XMFLOAT3(1, 0, 0);
	vertices[5].tangent  = XMFLOAT3(0, 0, 1);
	vertices[5].binormal = XMFLOAT3(0, 1, 0);

	vertices[6].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[6].texture  = XMFLOAT2(0, 1);
	vertices[6].normal   = XMFLOAT3(1, 0, 0);
	vertices[6].tangent  = XMFLOAT3(0, 0, 1);
	vertices[6].binormal = XMFLOAT3(0, 1, 0);

	vertices[7].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[7].texture  = XMFLOAT2(1, 1);
	vertices[7].normal   = XMFLOAT3(1, 0, 0);
	vertices[7].tangent  = XMFLOAT3(0, 0, 1);
	vertices[7].binormal = XMFLOAT3(0, 1, 0);

	vertices[8].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[8].texture  = XMFLOAT2(0, 0);
	vertices[8].normal   = XMFLOAT3(0, 0, 1);
	vertices[8].tangent  = XMFLOAT3(-1, 0, 0);
	vertices[8].binormal = XMFLOAT3(0, 1, -0);

	vertices[9].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[9].texture  = XMFLOAT2(1, 0);
	vertices[9].normal   = XMFLOAT3(0, 0, 1);
	vertices[9].tangent  = XMFLOAT3(-1, 0, 0);
	vertices[9].binormal = XMFLOAT3(0, 1, -0);

	vertices[10].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[10].texture  = XMFLOAT2(0, 1);
	vertices[10].normal   = XMFLOAT3(0, 0, 1);
	vertices[10].tangent  = XMFLOAT3(-1, 0, 0);
	vertices[10].binormal = XMFLOAT3(0, 1, -0);

	vertices[11].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[11].texture  = XMFLOAT2(1, 1);
	vertices[11].normal   = XMFLOAT3(0, 0, 1);
	vertices[11].tangent  = XMFLOAT3(-1, 0, 0);
	vertices[11].binormal = XMFLOAT3(0, 1, -0);

	vertices[12].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[12].texture  = XMFLOAT2(0, 0);
	vertices[12].normal   = XMFLOAT3(-1, 0, 0);
	vertices[12].tangent  = XMFLOAT3(0, 0, -1);
	vertices[12].binormal = XMFLOAT3(0, 1, 0);

	vertices[13].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[13].texture  = XMFLOAT2(1, 0);
	vertices[13].normal   = XMFLOAT3(-1, 0, 0);
	vertices[13].tangent  = XMFLOAT3(0, 0, -1);
	vertices[13].binormal = XMFLOAT3(0, 1, 0);

	vertices[14].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[14].texture  = XMFLOAT2(0, 1);
	vertices[14].normal   = XMFLOAT3(-1, 0, 0);
	vertices[14].tangent  = XMFLOAT3(0, 0, -1);
	vertices[14].binormal = XMFLOAT3(0, 1, 0);

	vertices[15].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[15].texture  = XMFLOAT2(1, 1);
	vertices[15].normal   = XMFLOAT3(-1, 0, 0);
	vertices[15].tangent  = XMFLOAT3(0, 0, -1);
	vertices[15].binormal = XMFLOAT3(0, 1, 0);

	vertices[16].position = XMFLOAT3(-0.5, 0.5, 0.5);
	vertices[16].texture  = XMFLOAT2(0, 0);
	vertices[16].normal   = XMFLOAT3(0, 1, 0);
	vertices[16].tangent  = XMFLOAT3(1, 0, 0);
	vertices[16].binormal = XMFLOAT3(0, 0, 1);

	vertices[17].position = XMFLOAT3(0.5, 0.5, 0.5);
	vertices[17].texture  = XMFLOAT2(1, 0);
	vertices[17].normal   = XMFLOAT3(0, 1, 0);
	vertices[17].tangent  = XMFLOAT3(1, 0, 0);
	vertices[17].binormal = XMFLOAT3(0, 0, 1);

	vertices[18].position = XMFLOAT3(-0.5, 0.5, -0.5);
	vertices[18].texture  = XMFLOAT2(0, 1);
	vertices[18].normal   = XMFLOAT3(0, 1, 0);
	vertices[18].tangent  = XMFLOAT3(1, 0, 0);
	vertices[18].binormal = XMFLOAT3(0, 0, 1);

	vertices[19].position = XMFLOAT3(0.5, 0.5, -0.5);
	vertices[19].texture  = XMFLOAT2(1, 1);
	vertices[19].normal   = XMFLOAT3(0, 1, 0);
	vertices[19].tangent  = XMFLOAT3(1, 0, 0);
	vertices[19].binormal = XMFLOAT3(0, 0, 1);

	vertices[20].position = XMFLOAT3(-0.5, -0.5, -0.5);
	vertices[20].texture  = XMFLOAT2(0, 0);
	vertices[20].normal   = XMFLOAT3(0, -1, 0);
	vertices[20].tangent  = XMFLOAT3(1, 0, 0);
	vertices[20].binormal = XMFLOAT3(0, 0, -1);

	vertices[21].position = XMFLOAT3(0.5, -0.5, -0.5);
	vertices[21].texture  = XMFLOAT2(1, 0);
	vertices[21].normal   = XMFLOAT3(0, -1, 0);
	vertices[21].tangent  = XMFLOAT3(1, 0, 0);
	vertices[21].binormal = XMFLOAT3(0, 0, -1);

	vertices[22].position = XMFLOAT3(-0.5, -0.5, 0.5);
	vertices[22].texture  = XMFLOAT2(0, 1);
	vertices[22].normal   = XMFLOAT3(0, -1, 0);
	vertices[22].tangent  = XMFLOAT3(1, 0, 0);
	vertices[22].binormal = XMFLOAT3(0, 0, -1);

	vertices[23].position = XMFLOAT3(0.5, -0.5, 0.5);
	vertices[23].texture  = XMFLOAT2(1, 1);
	vertices[23].normal   = XMFLOAT3(0, -1, 0);
	vertices[23].tangent  = XMFLOAT3(1, 0, 0);
	vertices[23].binormal = XMFLOAT3(0, 0, -1);

	ID3D11Device* device = Systems::dxManager->GetDevice();

	// create the descriptions and rasourcedata to buffers
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * 36;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	HRESULT result;

	// create vertex and index buffers
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result))
		DX_ERROR::PrintError(result, "failed to create vertex buffer in skybox.cpp");

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result))
		DX_ERROR::PrintError(result,"failed to create index buffer in skybox.cpp");
}

void SkyBox::LoadCubemap(wchar_t* file) 
{
	// remove the old cubemap texture if one exist
	if (_texture != nullptr)
		_texture->Release();

	// create cubemap from file
	HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->GetDevice(), file, NULL, &_texture);

	// get and print error message if failed
	if (FAILED(result))						
		DX_ERROR::PrintError(result, (std::string("failed to create cubemap with filename ") + DX_ERROR::ConvertFromWString(file)).c_str());
}

void SkyBox::Render() 
{
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent* camera = Systems::cameraManager->GetCurrentCameraGame();

	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexData);
	unsigned int offset = 0;

	// Set the vertex buffer 
	devCon->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Set the index buffer 
	devCon->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set shaders			
	devCon->VSSetShader(_vertexShader, NULL, 0);
	devCon->PSSetShader(_pixelShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_texture);

	// render with opaque blend
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);

	// use no culling
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	// only render non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);

	// constantbuffer structure
	ConstantVertex vertexData;
	
	// get and transpose the position matrix of camera transform
	XMFLOAT4X4 matrixPosition = camera->GetComponent<TransformComponent>()->GetPositionMatrix();	
	XMStoreFloat4x4(&matrixPosition, XMMatrixTranspose(XMLoadFloat4x4(&matrixPosition)));
		
	// set vertices
	vertexData.world      = matrixPosition;
	vertexData.view       = camera->GetViewMatrix();
	vertexData.projection = camera->GetProjectionMatrix();

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

	// draw
	devCon->DrawIndexed(36, 0, 0);

	// enable rendering of all pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);

	// set back to backcull
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::BACKCULL);
}