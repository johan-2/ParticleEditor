#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Color32.h"

using namespace DirectX;
class Mesh;
class TransformComponent;

enum SKY_DOME_RENDER_MODE
{
	CUBEMAP_SIMPLE,
	THREE_LAYER_COLOR_BLEND,
	CUBEMAP_COLOR_BLEND,
};

class SkyBox
{
public:
	SkyBox(const wchar_t* textureFile, SKY_DOME_RENDER_MODE mode);
	~SkyBox();
	
	// load cubemap
	void LoadCubemap(const wchar_t* file);

	// upload buffers and render
	void Render(bool useReflectViewMatrix = false);

	// set the skybox active/inactive
	void setActive(bool active) { _isActive = active; }

	// change the render mode
	void SetRenderMode(SKY_DOME_RENDER_MODE mode) { _RENDER_MODE = mode; }

	// set sun properties
	void SetSunDirectionTransformPtr(TransformComponent* direction) { _sun.directionPtr = direction; }
	void SetSunDirection(XMFLOAT3 direction)                        { _sun.direction = direction; }
	void SetSunDistance(float distance)                             { _sun.distance = XMFLOAT3(distance, distance, distance); }

	// set skyColorProperties
	// colors is declared in 0 - 255 range
	// the layer fraction is set between 0 - 100 and these should not overlap between the layers
	// ex 20, 60, 80 will set layer 1 color from 0 to 20 % on the skyDome 
	// layer 1 and 2 will blend between 20 - 60 % of the dome
	// layer2 and 3 will blend between 60 - 80 % of the dome and layer 3 will cover the final 20%
	void SetThreeLayerColorBlendSettings(XMFLOAT4 bottomColor, XMFLOAT4 midColor, XMFLOAT4 topColor) { _skyColorLayers.bottomColor = bottomColor; 
	                                                                                                   _skyColorLayers.midColor    = midColor; 
	                                                                                                   _skyColorLayers.topColor    = topColor; }

	// set color and blend properties for render mode
	// that uses cubemap at bottom or top, mid color and a color at the bottom or top
	// the blend properties is set in range 0 - 100 where 0 is at the bottom and 100 at the top
	void SetCubeMapColorBlendSettings(XMFLOAT4 bottomTopColor, XMFLOAT4 midColor, float bottomBlend, float midBlend, float topBlend, bool topIsCubeMap) { _cubeMapColorBlend.bottomTopColor = bottomTopColor;
		                                                                                                                                                  _cubeMapColorBlend.midColor       = midColor;
			                                                                                                                                              _cubeMapColorBlend.bottomBlend    = bottomBlend;
			                                                                                                                                              _cubeMapColorBlend.midBlend       = midBlend;
			                                                                                                                                              _cubeMapColorBlend.topBlend       = topBlend;
			                                                                                                                                              _cubeMapColorBlend.topIsCubeMap   = topIsCubeMap; }

private:

	// hold all sun data
	struct Sun
	{
		// billboarded 2D bitmap
		Mesh* mesh;

		XMFLOAT4X4 positionMatrix;
		XMFLOAT3   position;
		XMFLOAT3   direction;
		XMFLOAT3   distance;
		float      relativeHeight;

		// will use the forward direction of transform as sun direction
		TransformComponent* directionPtr;
	};

	struct ColorThreeLayerBlendData
	{
		XMFLOAT4 bottomColor;
		XMFLOAT4 midColor;
		XMFLOAT4 topColor;
	};

	struct CubeMapColorBlend
	{
		XMFLOAT4 bottomTopColor;
		XMFLOAT4 midColor;
		float    bottomBlend;
		float    midBlend;
		float    topBlend;
		bool     topIsCubeMap;
	};

	// create box mesh
	void CreateMeshes();
	void CaluclateSunMatrix(XMFLOAT3 cameraPosition);

	// render functions
	void RenderCubeMapSimple(bool useReflectMatrix);
	void RenderCubeMapColorBlend(bool useReflectMatrix);
	void RenderBlendedColors(bool useReflectMatrix);
	void RenderSun(bool useReflectMatrix);

	// constant buffers
	ID3D11Buffer* _constantBufferVertex;
	ID3D11Buffer* _constantBufferColorBlendPixel;
	ID3D11Buffer* _constantSunPixel;
	ID3D11Buffer* _constantBufferCubeMapBlendPixel;

	// compiled shaders
	ID3D11VertexShader* _vertexDomeCubeMapShader;
	ID3D11PixelShader*  _pixelDomeCubeMapShader;
	ID3D11VertexShader* _vertexDomeColorBlendShader;
	ID3D11PixelShader*  _pixelDomeColorBlendShader;
	ID3D11VertexShader* _vertexDomeCubeMapBlendShader;
	ID3D11PixelShader*  _pixelDomeCubeMapBlendShader;
	ID3D11VertexShader* _vertexSunShader;
	ID3D11PixelShader*  _pixelSunShader;

	// the shader bytecode
	ID3D10Blob* _vertexDomeCubeMapShaderByteCode;
	ID3D10Blob* _pixelDomeCubeMapShaderByteCode;
	ID3D10Blob* _vertexDomeColorBlendShaderByteCode;
	ID3D10Blob* _pixelDomeColorBlendShaderByteCode;
	ID3D10Blob* _vertexDomeCubeMapBlendShaderByteCode;
	ID3D10Blob* _pixelDomeCubeMapBlendShaderByteCode;
	ID3D10Blob* _vertexSunShaderByteCode;
	ID3D10Blob* _pixelSunShaderByteCode;

	// cubemap texture if renders in simple cubemap mode
	ID3D11ShaderResourceView* _cubeMap;

	// is skybox active
	bool _isActive;

	// sphere mesh
	Mesh* _domeMesh;

	// colors to use if renders in color mode
	ColorThreeLayerBlendData _skyColorLayers;
	CubeMapColorBlend        _cubeMapColorBlend;

	// sun data
	Sun _sun;

	SKY_DOME_RENDER_MODE _RENDER_MODE;

	// vertex constant buffer
	struct ConstantVertex
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct ConstantColorBlendPixel
	{
		XMFLOAT4 bottom;
		XMFLOAT4 mid;
		XMFLOAT4 top;
	};

	struct ConstantSunPixel
	{
		float sunDot;
		XMFLOAT3 pad;
	};

	struct ConstantCubeMapColorBlendPixel
	{
		XMFLOAT4 topBottomColor;
		XMFLOAT4 midColor;
		float    bottomBlend;
		float    midBlend;
		float    topBlend;
		int      cubeMapIsTop;
	};
};

