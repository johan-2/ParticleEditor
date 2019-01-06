#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class ScreenQuad;
class RenderToTexture;

class PostProcessingShader
{
public:
	PostProcessingShader();
	~PostProcessingShader();

	// renders all meshes to the depth map
	void Render(ScreenQuad* quad, ID3D11ShaderResourceView* SceneImage, ID3D11ShaderResourceView* sceneDepth);

	// get byte code from shaders
	ID3D10Blob*& GetVertexShaderByteCode() { return _vertexPostProcessingShaderByteCode; }
	ID3D10Blob*& GetPixelShaderByteCode()  { return _pixelPostProcessingShaderByteCode; }

	void CreateBloomBlurRenderTextures();
	void CreateBrightnessRenderTexture();
	void createDofRenderTextures();

private:

	ID3D11ShaderResourceView* RenderBlurMaps(ID3D11ShaderResourceView* imageToBlur, bool twoPass, float scaleDown1, float scaleDown2, RenderToTexture* h1, RenderToTexture* v1, RenderToTexture* h2, RenderToTexture* v2);
	void RenderBrightnessMap(ID3D11ShaderResourceView* originalImage);
	void RenderFinal(ID3D11ShaderResourceView* SceneImage, ID3D11ShaderResourceView* sceneDepth);

	// compiled shaders
	ID3D11VertexShader* _vertexBlurShader;
	ID3D11PixelShader*  _pixelBlurShader;
	ID3D11VertexShader* _vertexBrightnessShader;
	ID3D11PixelShader*  _pixelBrightnessShader;
	ID3D11VertexShader* _vertexPostProcessingShader;
	ID3D11PixelShader*  _pixelPostProcessingShader;
	
	// the shader bytecode
	ID3D10Blob* _vertexPostProcessingShaderByteCode;
	ID3D10Blob* _pixelPostProcessingShaderByteCode;
	ID3D10Blob* _vertexBlurShaderByteCode;
	ID3D10Blob* _pixelBlurShaderByteCode;
	ID3D10Blob* _vertexBrightnessShaderByteCode;
	ID3D10Blob* _pixelBrightnessShaderByteCode;

	// constant buffers
	ID3D11Buffer* _blurVertexConstant;
	ID3D11Buffer* _finalPixelConstant;

	// render textures 
	// TODO: all of these can be generated with compute shaders instead
	RenderToTexture* _brightnessMap;
	RenderToTexture* _bloomHorizontalBlurPass1;
	RenderToTexture* _bloomVerticalBlurPass1;
	RenderToTexture* _bloomHorizontalBlurPass2;
	RenderToTexture* _bloomVerticalBlurPass2;
	RenderToTexture* _dofHorizontalBlurPass;
	RenderToTexture* _dofVerticalBlurPass;

	// final input maps to post processing shader
	ID3D11ShaderResourceView* _bloomMap;
	ID3D11ShaderResourceView* _dofMap;

	struct ConstantBlurVertex
	{
		float screenWidth;
		float screenHeight;
		int   horizontalPass;
		int   pad; 
	};

	struct ConstantFinalPixel
	{
		int      applyBloom;
		float    bloomIntensity;
		int      applyDof;
		int      pad1;
		XMFLOAT2 startEndDofdst;
		XMFLOAT2 pad2;
	};
};

