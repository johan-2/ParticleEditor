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
	void Render(ScreenQuad* quad, ID3D11ShaderResourceView* SceneImage);

	// get byte code from shaders
	ID3D10Blob*& GetVertexShaderByteCode() { return _vertexPostProcessingShaderByteCode; }
	ID3D10Blob*& GetPixelShaderByteCode()  { return _pixelPostProcessingShaderByteCode; }

private:

	ID3D11ShaderResourceView* RenderBlurMaps(ID3D11ShaderResourceView* imageToBlur, bool twoPass, float scaleDown1, float scaleDown2);
	void RenderFinal(ID3D11ShaderResourceView* SceneImage);

	// compiled shaders
	ID3D11VertexShader* _vertexBlurShader;
	ID3D11PixelShader*  _pixelBlurShader;
	ID3D11VertexShader* _vertexPostProcessingShader;
	ID3D11PixelShader*  _pixelPostProcessingShader;
	
	// the shader bytecode
	ID3D10Blob* _vertexPostProcessingShaderByteCode;
	ID3D10Blob* _pixelPostProcessingShaderByteCode;
	ID3D10Blob* _vertexBlurShaderByteCode;
	ID3D10Blob* _pixelBlurShaderByteCode;

	// constant buffers
	ID3D11Buffer* _blurVertexConstant;

	// render textures 
	RenderToTexture* _horizontalBlurPass1;
	RenderToTexture* _verticalBlurPass1;
	RenderToTexture* _horizontalBlurPass2;
	RenderToTexture* _verticalBlurPass2;

	// final input maps to post processing shader
	ID3D11ShaderResourceView* _bloomMap;

	struct constantBlurPixel
	{
		float screenWidth;
		float screenHeight;
		int horizontalPass;
		int pad; 
	};
};

