#pragma once

#include <d3d11.h>
#include <vector>
#include "imgui.h"

class Mesh;
class QuadComponent;
class ParticleEmitterComponent;

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
	void Shutdown();
	
	void CreateShaders();
	void ShutdownShaders();

	void CreateVertexShader(const char* filePath, ID3D11VertexShader**, ID3D10Blob**);
	void CreatePixelShader(const char* filePath, ID3D11PixelShader**, ID3D10Blob**);

	void RenderAmbient(const std::vector<Mesh*>& meshes);
	void RenderDirectional(const std::vector<Mesh*>& meshes);
	void RenderPoint(const std::vector<Mesh*>& meshes);
	void RenderDepth(const std::vector<Mesh*>& meshes);
	void RenderDirectionalShadows(const std::vector<Mesh*>& meshes);
	void RenderGUI(ImDrawData* draw_data);

	void RenderQuadUI(const std::vector<QuadComponent*>& quads);
	
	void RenderSkyBox(D3DXMATRIX worldMatrix);
	void RenderParticles(const std::vector<ParticleEmitterComponent*>& emitters);

	void SetConstantBuffers();
	void SetInputLayout(INPUT_LAYOUT_TYPE type);
	
private:

	static ShaderManager* _instance;
	
	void UpdateVertexConstants(void* data, unsigned int size);
	void UpdatePixelConstants(void* data, unsigned int size);

	ID3D11VertexShader* _vertexDirectionalShader;
	ID3D11PixelShader* _pixelDirectionalShader;

	ID3D11VertexShader* _vertexPointShader;
	ID3D11PixelShader* _pixelPointShader;

	ID3D11VertexShader* _vertexDirectionalShadowsShader;
	ID3D11PixelShader* _pixelDirectionalShadowsShader;

	ID3D11VertexShader* _vertexAmbientShader;
	ID3D11PixelShader* _pixelAmbientShader;	

	ID3D11VertexShader* _vertexSpriteShader;
	ID3D11PixelShader* _pixelSpriteShader;
	
	ID3D11VertexShader* _vertexDepthShader;
	ID3D11PixelShader* _pixelDepthShader;

	ID3D11VertexShader* _vertexSkyBoxShader;
	ID3D11PixelShader* _pixelSkyBoxShader;

	ID3D11VertexShader* _vertexParticleShader;
	ID3D11PixelShader* _pixelParticleShader;

	ID3D11VertexShader* _vertexGUIShader;
	ID3D11PixelShader* _pixelGUIShader;

	ID3D11InputLayout* _inputlayout3D;
	ID3D11InputLayout* _inputlayout2D;
	ID3D11InputLayout* _inputlayoutParticle;
	ID3D11InputLayout* _inputlayoutGUI;

	ID3D11Buffer* _constantBufferVertex;
	ID3D11Buffer* _constantBufferPixel;
				
	// sprite constants
	struct ConstantQuadUIVertex
	{		
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	struct ConstantQuadUIPixel
	{
		D3DXVECTOR4 color;
	};

	//ambient constants
	struct ConstantAmbientVertex
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXVECTOR2 uvOffset;
		D3DXVECTOR2 pad;
	};
	struct ConstantAmbientPixel
	{
		D3DXVECTOR4 color;
	};
	
	// directional
	struct ConstantDirectionalVertex
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXVECTOR3 camPos;
		float pad1;
		D3DXVECTOR2 uvOffset;
		D3DXVECTOR2 pad2;
	};
	struct ConstantDirectionalPixel
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 specularColor;		
		D3DXVECTOR3 lightDir;
		float specularPower;		
	};

	struct ConstantDirectionalShadowVertex
	{		
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
		
		D3DXVECTOR3 camPos;
		float pad1;
		D3DXVECTOR2 uvOffset;
		D3DXVECTOR2 pad2;
		
	};
	struct ConstantDirectionalShadowPixel
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 specularColor;
		D3DXVECTOR3 lightDir;
		float specularPower;
	};

	//depth map constants
	struct ConstantDepthVertex
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	
	struct ConstantSkyBoxVertex
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct ConstantParticleVertex
	{
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	
	struct ConstantGUIVertex
	{
		float projection[4][4];
	};
	
	struct ConstantPointVertex
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
				
		D3DXVECTOR3 camPos;	
		float pad1;
		D3DXVECTOR2 uvOffset;
		D3DXVECTOR2 pad2;
	
	};

	struct ConstantPointPixel
	{
		D3DXVECTOR3 lightPosition;
		float radius;
		D3DXVECTOR3 color;
		float intensity;
		D3DXVECTOR3 specularColor;
		float specularPower;

		float attConstant;
		float attLinear;
		float attExponential;
		int numLights;		
	};
	
};

