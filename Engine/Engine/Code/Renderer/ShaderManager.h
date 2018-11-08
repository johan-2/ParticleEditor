#pragma once

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include "imgui.h"

using namespace DirectX;

class Mesh;
class QuadComponent;
class ParticleSystemComponent;
class GBuffer;

enum INPUT_LAYOUT_TYPE
{
	LAYOUT3D,
	LAYOUT2D,
	LAYOUTPARTICLE
};

class ShaderManager
{	
public:
	
	static ShaderManager& GetInstance();

	ShaderManager();
	~ShaderManager();	
		
	void Initialize();
	
	void CreateVertexShader(LPCWSTR filePath, ID3D11VertexShader*& shader, ID3D10Blob*& buffer);
	void CreatePixelShader( LPCWSTR filePath, ID3D11PixelShader*& shader,  ID3D10Blob*& buffer);

	// forward rendering for alpha meshes
	void RenderAmbientAlpha(Mesh*& mesh);
	void RenderDirectionalAlpha(Mesh*& mesh);
	void RenderPointAlpha(Mesh*& mesh);
	void RenderDirectionalShadowsAlpha(Mesh*& mesh);

	// deferred rendering
	void RenderDepth(const std::vector<Mesh*>& meshes);
	void RenderGeometry(const std::vector<Mesh*>& meshes);
	void RenderLights(GBuffer*& gBuffer);

	// 2d rendering
	void RenderQuadUI(const std::vector<QuadComponent*>& quads);
	void RenderGUI(ImDrawData* draw_data);
	
	// skybox Particle rendering
	void RenderSkyBox(XMFLOAT4X4 worldMatrix);
	void RenderParticles(const std::vector<ParticleSystemComponent*>& emitters);

	void SetConstantBuffers();
	void SetInputLayout(INPUT_LAYOUT_TYPE type);
	
private:

	static ShaderManager* _instance;
	
	void UpdateConstantBuffer(void* data, unsigned int size, ID3D11Buffer*& buffer);

	ID3D11VertexShader* _vertexDirectionalShader;
	ID3D11PixelShader*  _pixelDirectionalShader;

	ID3D11VertexShader* _vertexPointShader;
	ID3D11PixelShader*  _pixelPointShader;

	ID3D11VertexShader* _vertexDirectionalShadowsShader;
	ID3D11PixelShader*  _pixelDirectionalShadowsShader;

	ID3D11VertexShader* _vertexAmbientShader;
	ID3D11PixelShader*  _pixelAmbientShader;	

	ID3D11VertexShader* _vertexSpriteShader;
	ID3D11PixelShader*  _pixelSpriteShader;
	
	ID3D11VertexShader* _vertexDepthShader;
	ID3D11PixelShader*  _pixelDepthShader;

	ID3D11VertexShader* _vertexSkyBoxShader;
	ID3D11PixelShader*  _pixelSkyBoxShader;

	ID3D11VertexShader* _vertexParticleShader;
	ID3D11PixelShader*  _pixelParticleShader;

	ID3D11VertexShader* _vertexGUIShader;
	ID3D11PixelShader*  _pixelGUIShader;

	ID3D11VertexShader* _vertexGeometryShader;
	ID3D11PixelShader*  _pixelGeometryShader;

	ID3D11VertexShader* _vertexLightShader;
	ID3D11PixelShader*  _pixelLightShader;

	ID3D11InputLayout* _inputlayout3D;
	ID3D11InputLayout* _inputlayout2D;
	ID3D11InputLayout* _inputlayoutParticle;
	ID3D11InputLayout* _inputlayoutGUI;

	ID3D11Buffer* _constantBufferVertex;
	ID3D11Buffer* _constantBufferPixel;

	// deferred buffers
	ID3D11Buffer* _constantBufferDefPoint;
	ID3D11Buffer* _constantBufferDefAmbient;
	ID3D11Buffer* _constantBufferDefDirectional;
				
	struct ConstantQuadUIVertex
	{		
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	struct ConstantQuadUIPixel
	{
		XMFLOAT4 color;
	};

	struct ConstantAmbientVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT2   uvOffset;
		XMFLOAT2   pad;
	};
	struct ConstantAmbientPixel
	{
		XMFLOAT4 color;
	};
	
	struct ConstantDirectionalVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT3   camPos;
		float      pad1;
		XMFLOAT2   uvOffset;
		XMFLOAT2   pad2;
	};
	struct ConstantDirectionalPixel
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT4 specularColor;		
		XMFLOAT3 lightDir;
		float    specularPower;		
	};

	struct ConstantDirectionalShadowVertex
	{		
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT4X4 lightView;
		XMFLOAT4X4 lightProjection;
		
		XMFLOAT3 camPos;
		float    pad1;
		XMFLOAT2 uvOffset;
		XMFLOAT2 pad2;	
	};
	struct ConstantDirectionalShadowPixel
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT4 specularColor;
		XMFLOAT3 lightDir;
		float    specularPower;
	};

	struct ConstantDepthVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	
	struct ConstantSkyBoxVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct ConstantParticleVertex
	{
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};
	
	struct ConstantGUIVertex
	{
		float projection[4][4];
	};
	
	struct ConstantPointVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
				
		XMFLOAT3 camPos;	
		float    pad1;
		XMFLOAT2 uvOffset;
		XMFLOAT2 pad2;
	
	};

	struct ConstantPointPixel
	{
		XMFLOAT3 lightPosition;
		float    radius;
		XMFLOAT3 color;
		float    intensity;
		XMFLOAT3 specularColor;
		float    specularPower;

		float attConstant;
		float attLinear;
		float attExponential;
		int   numLights;		
	};

	struct ConstantGeometryVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
		XMFLOAT2   uvOffset;
		XMFLOAT2   pad;
	};
	
	struct ConstantDeferredAmbient
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 cameraPosition;
	};

	struct ConstantDeferredDirectional
	{
		XMFLOAT4X4 lightViewProj;
		XMFLOAT4   lightColor;
		XMFLOAT4   lightSpecularColor;
		XMFLOAT3   lightDirection;
		float      lightSpecularpower;
	};

	struct ConstantDeferredPoint
	{
		XMFLOAT3 lightPosition;
		float    radius;
		XMFLOAT3 color;
		float    intensity;
		XMFLOAT3 specularColor;
		float    specularPower;

		float attConstant;
		float attLinear;
		float attExponential;
		int   numLights;
	};
};

