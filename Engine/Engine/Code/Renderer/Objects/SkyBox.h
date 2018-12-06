#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;
class Mesh;
class TransformComponent;

class SkyBox
{
public:
	SkyBox(const wchar_t* textureFile);
	~SkyBox();
	
	// load cubemap
	void LoadCubemap(const wchar_t* file);

	// upload buffers and render
	void Render(bool useReflectViewMatrix = false);

	// set the skybox active/inactive
	void setActive(bool active) { _isActive = active; }

	// set sun properties
	void SetSunDirectionTransformPtr(TransformComponent* direction) { _sun.directionPtr = direction; }
	void SetSunDirection(XMFLOAT3 direction)                        { _sun.direction = direction; }
	void SetSunDistance(float distance)                             { _sun.distance = XMFLOAT3(distance, distance, distance); }
	
private:

	// hold all sun data
	struct Sun
	{
		Mesh* mesh;

		XMFLOAT4X4 positionMatrix;
		XMFLOAT4X4 rotationMatrix;
		XMFLOAT4X4 scaleMatrix;
		XMFLOAT4X4 worldMatrix;
		XMFLOAT3   position;
		XMFLOAT3   scale;
		XMFLOAT3   rotation;
		XMFLOAT3   direction;
		XMFLOAT3   distance;

		// will use the forward direction of transform as sun direction
		TransformComponent* directionPtr;
	};

	// create box mesh
	void CreateBox();
	void CaluclateSunMatrix(XMFLOAT3 cameraPosition);

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;

	// compiled shaders
	ID3D11VertexShader* _vertexDomeShader;
	ID3D11PixelShader*  _pixelDomeShader;

	// compiled shaders
	ID3D11VertexShader* _vertexSunShader;
	ID3D11PixelShader*  _pixelSunShader;

	// the shader bytecode
	ID3D10Blob* _vertexDomeShaderByteCode;
	ID3D10Blob* _pixelDomeShaderByteCode;

	// the shader bytecode
	ID3D10Blob* _vertexSunShaderByteCode;
	ID3D10Blob* _pixelSunShaderByteCode;

	// cubemap texture
	ID3D11ShaderResourceView* _cubeMap;

	// is skybox active
	bool _isActive;

	Mesh* _domeMesh;

	Sun _sun;

	// vertex constant buffer
	struct ConstantVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	

};

