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



class ShaderManager
{	
public:
	
	static ShaderManager& GetInstance();

	ShaderManager();
	~ShaderManager();	
		
	void Initialize();

	void RenderGUI(ImDrawData* draw_data);
	void RenderSkyBox(XMFLOAT4X4 worldMatrix);
	void RenderParticles(const std::vector<ParticleSystemComponent*>& emitters);
		
private:

	static ShaderManager* _instance;
	

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
	
	

	ID3D11VertexShader* _vertexSkyBoxShader;
	ID3D11PixelShader*  _pixelSkyBoxShader;

	ID3D11VertexShader* _vertexParticleShader;
	ID3D11PixelShader*  _pixelParticleShader;

	ID3D11VertexShader* _vertexGUIShader;
	ID3D11PixelShader*  _pixelGUIShader;

	

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
	
	
};

