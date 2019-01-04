#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Color32.h"

using namespace DirectX;
class Mesh;
class TransformComponent;
class Entity;

enum SKY_DOME_RENDER_MODE
{
	CUBEMAP,
	THREE_LAYER_COLOR_BLEND,
};

class SkyDome
{
public:
	SkyDome(const wchar_t* textureFile, SKY_DOME_RENDER_MODE mode);
	~SkyDome();

	// load cubemap
	void LoadCubemap(const wchar_t* file);

	void Update(const float& delta);

	// upload buffers and render
	void Render(bool noMask = false);

	// set the skybox active/inactive
	void setActive(bool active) { _isActive = active; }

	// change the render mode
	void SetRenderMode(SKY_DOME_RENDER_MODE mode) { _RENDER_MODE = mode; }

	// set skyColorProperties
	// colors is declared in 0 - 255 range
	// the layer fraction is set between 0 - 100 and is stored in the alpha channel
	// ex 20, 60, 80 will set layer 1 color from 0 to 20 % on the skyDome
	// layer 1 and 2 will blend between 20 - 60 % of the dome
	// layer2 and 3 will blend between 60 - 80 % of the dome and layer 3 will cover the final 20%
	void SetSkyColorLayers(XMFLOAT4 bottomColor, XMFLOAT4 midColor, XMFLOAT4 topColor) {
		_skyColorLayers.bottomColor = bottomColor;
		_skyColorLayers.midColor    = midColor;
		_skyColorLayers.topColor    = topColor;
	}

private:

	// hold all sun/moon specific settings
	struct SkyObject
	{
		// billboarded 2D bitmap
		Mesh* mesh;

		XMFLOAT4X4 positionMatrix;
		XMFLOAT3   distance;
		XMFLOAT3   colorTint = XMFLOAT3(1, 1, 1);
		XMFLOAT3   dayColorTint;
		XMFLOAT3   sunsetColorTint;
		XMFLOAT2   beginEndFade;
		XMFLOAT2   minMaxDst;
		XMFLOAT2   beginEndDstLerp;
		XMFLOAT2   beginEndColorBlend;
		float      relativeHeight;

		Entity* entity;
		TransformComponent* transform;
	};

	struct SunMoon
	{
		SkyObject sun;
		SkyObject moon;
	};

	struct DynamicDomeSettings
	{
		// is this system enabled
		bool isEnabled = true;

		// only used from dev tools
		float speedMultiplier = 1.0f;

		// cycle values
		float cycleInSec = 20.0f;
		float cycleTimer = 0;

		float switchToMoonLightThreshold = -0.1f;

		// start/end blend values
		XMFLOAT2 sunsetLightColorStartEndBlend  = XMFLOAT2(0.2f, 0.0f);
		XMFLOAT2 nightLightColorStartEndBlend   = XMFLOAT2(0.0f, -0.1f);
		XMFLOAT2 sunsetTopSkyColorStartEndBlend = XMFLOAT2(0.5f, 0.1f);
		XMFLOAT2 nightTopSkyColorStartEndBlend  = XMFLOAT2(0.1f, -0.3f);
		XMFLOAT2 sunsetMidSkyColorStartEndBlend = XMFLOAT2(0.45f, 0.0f);
		XMFLOAT2 nightMidSkyColorStartEndBlend  = XMFLOAT2(0.0f, -0.25f);

		// start == to beginning to fade out
		XMFLOAT2 dayLightStartEndfade   = XMFLOAT2(0.0f, -0.1f);
		XMFLOAT2 nightLightStartEndfade = XMFLOAT2(-0.2f, -0.1f);

		// translation values for shadow rendering camera
		XMFLOAT3 shadowMapDistance = XMFLOAT3(300, 300, 300);
		XMFLOAT3 startRotation     = XMFLOAT3(0, 100.0f, 0);
		XMFLOAT3 endRotation       = XMFLOAT3(360.0f, 100.0f, 0.0f);

		// colors
		XMFLOAT4 normalDirLightColor = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f);
		XMFLOAT4 sunsetDirLightColor = XMFLOAT4(0.9935f, 0.07211f, 0.08812f, 1.0f);
		XMFLOAT4 nightDirLightColor  = XMFLOAT4(0.1f, 0.1f, 0.26f, 1.0f);
		XMFLOAT4 topSkyColorDay      = XMFLOAT4(0.082f, 0.352f, 0.984f, 1.0f);
		XMFLOAT4 topSkyColorSunSet   = XMFLOAT4(0.35f, 0.45f, 0.984f, 1.0f);
		XMFLOAT4 topSkyColorNight    = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		XMFLOAT4 midSkyColorDay      = XMFLOAT4(0.62f, 0.6f, 0.30f, 1.0f);
		XMFLOAT4 midSkyColorSunSet   = XMFLOAT4(0.9935f, 0.07211f, 0.08812f, 1.0f);
		XMFLOAT4 midSkyColorNight    = XMFLOAT4(0.109f, 0.035f, 0.1f, 1.0f);
	};

	struct SkyColorLayers
	{
		XMFLOAT4 bottomColor;
		XMFLOAT4 midColor;
		XMFLOAT4 topColor;
	};

	// create mesh
	void CreateMeshes();

	// get the sunmatrix
	void CaluclateSunMoonMatrix(XMFLOAT3 cameraPosition);

	// render functions
	void RenderCubeMap(bool useReflectMatrix);
	void RenderBlendedColors(bool useReflectMatrix);
	void RenderSunMoon(bool useReflectMatrix);

	// update functions
	void UpdateSunMoonTranslation(const float& delta);
	void UpdateSunMoonColors(const float& delta);
	void UpdateShadowLightTranslation(const float& delta);
	void UpdateShadowLightColor(const float& delta);
	void UpdateSkyColors(const float& delta);

	// helpers, these should be moved to some static math helper header
	float inverseLerp(float a, float b, float t);
	float lerpF(float a, float b, float f);
	void LerpColorRGB(XMFLOAT4& result, XMFLOAT4 colorA, XMFLOAT4 colorB, float startBlend, float endBlend, float fraction);
	void LerpColorRGB(XMFLOAT3& result, XMFLOAT3 colorA, XMFLOAT3 colorB, float startBlend, float endBlend, float fraction);

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
	ID3D11VertexShader* _vertexSunShader;
	ID3D11PixelShader*  _pixelSunShader;

	// the shader bytecode
	ID3D10Blob* _vertexDomeCubeMapShaderByteCode;
	ID3D10Blob* _pixelDomeCubeMapShaderByteCode;
	ID3D10Blob* _vertexDomeColorBlendShaderByteCode;
	ID3D10Blob* _pixelDomeColorBlendShaderByteCode;
	ID3D10Blob* _vertexSunShaderByteCode;
	ID3D10Blob* _pixelSunShaderByteCode;

	// cubemap texture if renders in simple cubemap mode
	ID3D11ShaderResourceView* _cubeMap;

	// is skybox active
	bool _isActive;

	// sphere mesh
	Mesh* _domeMesh;

	// colors to use if renders in color mode
	SkyColorLayers _skyColorLayers;

	// sun data
	SunMoon _sunMoon;

	DynamicDomeSettings _dynamicSky;

	SKY_DOME_RENDER_MODE _RENDER_MODE;

	// vertex constant buffer
	struct CBVertDome
	{
		XMFLOAT4X4 worldViewProj;
	};

	struct CBVertSun
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 Proj;
	};

	struct ConstantColorBlendPixel
	{
		XMFLOAT4 bottom;
		XMFLOAT4 mid;
		XMFLOAT4 top;
	};

	struct ConstantSunPixel
	{
		float    sunDot;
		XMFLOAT3 colorTint;
		XMFLOAT2 beginEndFade;
		XMFLOAT2 pad;
	};
};

