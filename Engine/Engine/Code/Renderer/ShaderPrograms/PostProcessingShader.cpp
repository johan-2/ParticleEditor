#include "PostProcessingShader.h"
#include "DXManager.h"
#include "Systems.h"
#include "ShaderHelpers.h"
#include "ScreenQuad.h"
#include "DXDepthStencilStates.h"
#include "DXBlendStates.h"
#include "RenderToTexture.h"
#include "SystemDefs.h"
#include "Entity.h"
#include "QuadComponent.h"

PostProcessingShader::PostProcessingShader()
{
	// create and compile shaders
	SHADER_HELPERS::CreateVertexShader(L"Shaders/PostProcess/vertexPostProcessing.vs", _vertexPostProcessingShader, _vertexPostProcessingShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"Shaders/PostProcess/pixelPostProcessing.ps",   _pixelPostProcessingShader,  _pixelPostProcessingShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"Shaders/PostProcess/vertexBlur.vs",           _vertexBlurShader,           _vertexBlurShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"Shaders/PostProcess/pixelBlur.ps",             _pixelBlurShader,            _pixelBlurShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"Shaders/PostProcess/vertexBrightness.vs",     _vertexBrightnessShader,     _vertexBrightnessShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"Shaders/PostProcess/pixelBrightness.ps",       _pixelBrightnessShader,      _pixelBrightnessShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_blurVertexConstant);
	SHADER_HELPERS::CreateConstantBuffer(_finalPixelConstant);

	// create render textures
	_brightnessMap       = new RenderToTexture(SCREEN_WIDTH, SCREEN_HEIGHT, false);
	
	CreateBlurRenderTextures();

	Entity* reflectionQuad = new Entity();
	reflectionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SCREEN_WIDTH * 0.78f, SCREEN_HEIGHT * 0.1f), XMFLOAT2(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.1f), L"");
	reflectionQuad->GetComponent<QuadComponent>()->SetTexture(_brightnessMap->GetRenderTargetSRV());
}

PostProcessingShader::~PostProcessingShader()
{
	_vertexPostProcessingShaderByteCode->Release();
	_pixelPostProcessingShaderByteCode->Release();

	_vertexPostProcessingShader->Release();
	_pixelPostProcessingShader->Release();
}

void PostProcessingShader::CreateBlurRenderTextures()
{
	if (_horizontalBlurPass1) delete _horizontalBlurPass1;
	if (_verticalBlurPass1)   delete _verticalBlurPass1;
	if (_horizontalBlurPass2) delete _horizontalBlurPass2;
	if (_verticalBlurPass2)   delete _verticalBlurPass2;

	_horizontalBlurPass1 = new RenderToTexture(SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, false);
	_verticalBlurPass1   = new RenderToTexture(SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, false);
	_horizontalBlurPass2 = new RenderToTexture(SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, false);
	_verticalBlurPass2   = new RenderToTexture(SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, false);
}

void PostProcessingShader::Render(ScreenQuad* quad, ID3D11ShaderResourceView* SceneImage)
{
	Systems::dxManager->BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);
	quad->UploadBuffers();

	if (PostProcessing::APPLY_BLOOM)
	{
		RenderBrightnessMap(SceneImage);

		// blur the brightness map
		_bloomMap = RenderBlurMaps(_brightnessMap->GetRenderTargetSRV(), PostProcessing::BLOOM_USE_TWO_PASS_BLUR, PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2);
	}

	RenderFinal(SceneImage);
}

ID3D11ShaderResourceView* PostProcessingShader::RenderBlurMaps(ID3D11ShaderResourceView* imageToBlur, bool twoPass, float scaleDown1, float scaleDown2)
{
	// get dx manager
	DXManager& DXM = *Systems::dxManager;

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set our shaders
	devCon->VSSetShader(_vertexBlurShader, NULL, 0);
	devCon->PSSetShader(_pixelBlurShader, NULL, 0);

	// the textures we will use as input
	ID3D11ShaderResourceView* hBlur1SRV = _horizontalBlurPass1->GetRenderTargetSRV();
	ID3D11ShaderResourceView* hBlur2SRV = _horizontalBlurPass2->GetRenderTargetSRV();
	ID3D11ShaderResourceView* vBlur1SRV = _verticalBlurPass1->GetRenderTargetSRV();
	
	// constantbuffer for vertex shader
	ConstantBlurVertex constantVertex;
	devCon->VSSetConstantBuffers(0, 1, &_blurVertexConstant);

	////////////////////////////////////////////////////////// HORIZONTAL PASS 1

	// set the horizontal blur render target 1
	_horizontalBlurPass1->ClearRenderTarget(0, 0, 0, 1, false);
	_horizontalBlurPass1->SetRendertarget(false, false);

	constantVertex.screenWidth    = SCREEN_WIDTH  / scaleDown1;
	constantVertex.screenHeight   = SCREEN_HEIGHT / scaleDown1;
	constantVertex.horizontalPass = 1;

	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set image to blur
	devCon->PSSetShaderResources(0, 1, &imageToBlur);

	// draw horizontal pass
	devCon->DrawIndexed(6, 0, 0);

	////////////////////////////////////////////////////////// VERTICAL PASS 1

	// set vertical blur render target 1
	_verticalBlurPass1->ClearRenderTarget(0, 0, 0, 1, false);
	_verticalBlurPass1->SetRendertarget(false, false);

	// change constant input
	constantVertex.horizontalPass = 0;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set horizontal blur texture
	devCon->PSSetShaderResources(0, 1, &hBlur1SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	// if we only are doing one blur pass return the texture from the first vertical pass
	if (!twoPass)
		return _verticalBlurPass1->GetRenderTargetSRV();

	////////////////////////////////////////////////////////// HORIZONTAL PASS 2

	// set horizontal blur render target 2
	_horizontalBlurPass2->ClearRenderTarget(0, 0, 0, 1, false);
	_horizontalBlurPass2->SetRendertarget(false, false);

	// change constants for pass 2
	constantVertex.horizontalPass = 1;
	constantVertex.screenWidth    = SCREEN_WIDTH  / scaleDown2;
	constantVertex.screenHeight   = SCREEN_HEIGHT / scaleDown2;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// get the horizontal blured texture from pass 1
	devCon->PSSetShaderResources(0, 1, &vBlur1SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	////////////////////////////////////////////////////////// VERTICAL PASS 2

	// set vertical blur render target 2
	_verticalBlurPass2->ClearRenderTarget(0, 0, 0, 1, false);
	_verticalBlurPass2->SetRendertarget(false, false);

	// change constants for vertical pass 2
	constantVertex.horizontalPass = 0;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set horizontal blur texture
	devCon->PSSetShaderResources(0, 1, &hBlur2SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	// return the final blurred image from vertical pass 2
	return _verticalBlurPass2->GetRenderTargetSRV();
}

void PostProcessingShader::RenderBrightnessMap(ID3D11ShaderResourceView* originalImage)
{
	// get dx manager
	DXManager& DXM = *Systems::dxManager;

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set the render target to output the brighnessmap
	_brightnessMap->ClearRenderTarget(0, 0, 0, 1, false);
	_brightnessMap->SetRendertarget(false, false);

	// set our shaders
	devCon->VSSetShader(_vertexBrightnessShader, NULL, 0);
	devCon->PSSetShader(_pixelBrightnessShader, NULL, 0);

	// set textures
	devCon->PSSetShaderResources(0, 1, &originalImage);

	// draw
	devCon->DrawIndexed(6, 0, 0);
}

void PostProcessingShader::RenderFinal(ID3D11ShaderResourceView* SceneImage)
{
	// get dx manager
	DXManager& DXM   = *Systems::dxManager;

	// set to defult backbuffer and render our final scene image
	DXM.SetBackBuffer();

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set our shaders
	devCon->VSSetShader(_vertexPostProcessingShader, NULL, 0);
	devCon->PSSetShader(_pixelPostProcessingShader, NULL, 0);

	devCon->PSSetConstantBuffers(0, 1, &_finalPixelConstant);

	// set pixel constants
	ConstantFinalPixel pixelConstant;
	pixelConstant.applyBloom     = PostProcessing::APPLY_BLOOM;
	pixelConstant.bloomIntensity = PostProcessing::BLOOM_INTENSITY;

	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixelConstant, sizeof(ConstantFinalPixel), _finalPixelConstant);

	// set textures
	ID3D11ShaderResourceView* texArray[2] = { SceneImage, _bloomMap };
	devCon->PSSetShaderResources(0, 2, texArray);

	// draw
	devCon->DrawIndexed(6, 0, 0);
}

