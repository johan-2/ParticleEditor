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
	SHADER_HELPERS::CreateComputeShader(L"Shaders/PostProcess/computeBrightness.cs",   _computeBrightnessShader,    _computeBrightnessShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_blurVertexConstant);
	SHADER_HELPERS::CreateConstantBuffer(_finalPixelConstant);

	// create render textures
	CreateBloomBlurRenderTextures();
	createDofRenderTextures();

	SHADER_HELPERS::CreateTexture2DUAVSRV(SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT, _brigtnessTex, _brigthnessSRV, _brightnessUAV);

	Entity* reflectionQuad = new Entity();
	reflectionQuad->AddComponent<QuadComponent>()->Init(XMFLOAT2(SystemSettings::SCREEN_WIDTH * 0.78f, SystemSettings::SCREEN_HEIGHT * 0.1f), XMFLOAT2(SystemSettings::SCREEN_WIDTH * 0.1f, SystemSettings::SCREEN_HEIGHT * 0.1f), L"");
	reflectionQuad->GetComponent<QuadComponent>()->SetTexture(_brigthnessSRV);
}

PostProcessingShader::~PostProcessingShader()
{
	_vertexPostProcessingShader->Release();
	_pixelPostProcessingShader->Release();
	_vertexBlurShader->Release();
	_pixelBlurShader->Release();
	_computeBrightnessShader->Release();

	_vertexPostProcessingShaderByteCode->Release();
	_pixelPostProcessingShaderByteCode->Release();
	_vertexBlurShaderByteCode->Release();
	_pixelBlurShaderByteCode->Release();
	_computeBrightnessShaderByteCode->Release();
}

void PostProcessingShader::CreateBloomBlurRenderTextures()
{
	if (_bloomHorizontalBlurPass1) delete _bloomHorizontalBlurPass1;
	if (_bloomVerticalBlurPass1)   delete _bloomVerticalBlurPass1;
	if (_bloomHorizontalBlurPass2) delete _bloomHorizontalBlurPass2;
	if (_bloomVerticalBlurPass2)   delete _bloomVerticalBlurPass2;

	_bloomHorizontalBlurPass1 = new RenderToTexture(SystemSettings::SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, SystemSettings::SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, false, SystemSettings::USE_HDR);
	_bloomVerticalBlurPass1   = new RenderToTexture(SystemSettings::SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, SystemSettings::SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, false, SystemSettings::USE_HDR);
	_bloomHorizontalBlurPass2 = new RenderToTexture(SystemSettings::SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, SystemSettings::SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, false, SystemSettings::USE_HDR);
	_bloomVerticalBlurPass2   = new RenderToTexture(SystemSettings::SCREEN_WIDTH / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, SystemSettings::SCREEN_HEIGHT / PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2, false, SystemSettings::USE_HDR);
}

void PostProcessingShader::createDofRenderTextures()
{
	_dofHorizontalBlurPass = new RenderToTexture(SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT, false, SystemSettings::USE_HDR);
	_dofVerticalBlurPass   = new RenderToTexture(SystemSettings::SCREEN_WIDTH, SystemSettings::SCREEN_HEIGHT, false, SystemSettings::USE_HDR);
}

void PostProcessingShader::Render(ScreenQuad* quad, ID3D11ShaderResourceView* SceneImage, ID3D11ShaderResourceView* sceneDepth)
{
	DXManager& DXM = *Systems::dxManager;
	DXM.BlendStates()->SetBlendState(BLEND_STATE::BLEND_OPAQUE);
	quad->UploadBuffers();

	// unbind our main rendertarget so we can use the SRV as input 
	// when generating our post process scene filters
	DXM.SetNullRenderTarget();

	if (PostProcessing::APPLY_BLOOM)
	{		
		// compute brightness map
		ComputeBrightnessMap(SceneImage);

		// blur the brightness map
		_bloomMap = RenderBlurMaps(_brigthnessSRV, 
			PostProcessing::BLOOM_USE_TWO_PASS_BLUR,
			PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_1, 
			PostProcessing::BLOOM_BLUR_SCALE_DOWN_PASS_2,
			_bloomHorizontalBlurPass1, 
			_bloomVerticalBlurPass1,
			_bloomHorizontalBlurPass2,
			_bloomVerticalBlurPass2);
	}

	if (PostProcessing::APPLY_DEPTH_OF_FIELD)
		_dofMap = RenderBlurMaps(SceneImage, false, 1, 1, _dofHorizontalBlurPass, _dofVerticalBlurPass, nullptr, nullptr);

	RenderFinal(SceneImage, sceneDepth);
}

void PostProcessingShader::ComputeBrightnessMap(ID3D11ShaderResourceView* originalImage)
{
	// get devicecontext
	ID3D11DeviceContext* devCon = Systems::dxManager->GetDeviceCon();

	devCon->CSSetShader(_computeBrightnessShader, NULL, 0);
	devCon->CSSetShaderResources(0, 1, &originalImage);
	devCon->CSSetUnorderedAccessViews(0, 1, &_brightnessUAV, 0);

	// shader is set to [32, 32, 1] threads per group meaning we have 32 * 32 = 1024 threads per group
	// if our screen is 1920 * 1080 we need 2073600 threads to cover all pixels
	// 1920 / 32 will result in 60 thread groups, 1080 / 32 will result in 33.75 thread groups, 60 * 33.75 = 2025 thread groups
	// 2025 groups * 1024 threads = 2073600 threads
	// in case of a 2d texture like this we should just think (width / declared therads.x, height / declared threads.y) and we will and up with the correct amount of threads to use	
	devCon->Dispatch(SystemSettings::SCREEN_WIDTH / 32, SystemSettings::SCREEN_HEIGHT / 32, 1);

	// unbind so we can use resources as input in next stages
	ID3D11ShaderResourceView* nullSRV[] = { NULL };
	devCon->CSSetShaderResources(0, 1, nullSRV);

	ID3D11UnorderedAccessView* nullUAV[] = { NULL };
	devCon->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
}

ID3D11ShaderResourceView* PostProcessingShader::RenderBlurMaps(ID3D11ShaderResourceView* imageToBlur, bool twoPass, float scaleDown1, float scaleDown2, RenderToTexture* h1, RenderToTexture* v1, RenderToTexture* h2, RenderToTexture* v2)
{
	// get dx manager
	DXManager& DXM = *Systems::dxManager;

	// get devicecontext
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// set our shaders
	devCon->VSSetShader(_vertexBlurShader, NULL, 0);
	devCon->PSSetShader(_pixelBlurShader, NULL, 0);

	// the textures we will use as input
	ID3D11ShaderResourceView* hBlur1SRV = h1->GetRenderTargetSRV();
	ID3D11ShaderResourceView* vBlur1SRV = v1->GetRenderTargetSRV();
	
	// constantbuffer for vertex shader
	ConstantBlurVertex constantVertex;
	devCon->VSSetConstantBuffers(0, 1, &_blurVertexConstant);

	////////////////////////////////////////////////////////// HORIZONTAL PASS 1

	// set the horizontal blur render target 1
	h1->ClearRenderTarget(0, 0, 0, 1, false);
	h1->SetRendertarget(false, false);

	constantVertex.screenWidth    = SystemSettings::SCREEN_WIDTH  / scaleDown1;
	constantVertex.screenHeight   = SystemSettings::SCREEN_HEIGHT / scaleDown1;
	constantVertex.horizontalPass = 1;

	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set image to blur
	devCon->PSSetShaderResources(0, 1, &imageToBlur);

	// draw horizontal pass
	devCon->DrawIndexed(6, 0, 0);

	////////////////////////////////////////////////////////// VERTICAL PASS 1

	// set vertical blur render target 1
	v1->ClearRenderTarget(0, 0, 0, 1, false);
	v1->SetRendertarget(false, false);

	// change constant input
	constantVertex.horizontalPass = 0;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set horizontal blur texture
	devCon->PSSetShaderResources(0, 1, &hBlur1SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	// if we only are doing one blur pass return the texture from the first vertical pass
	if (!twoPass)
		return v1->GetRenderTargetSRV();

	////////////////////////////////////////////////////////// HORIZONTAL PASS 2

	ID3D11ShaderResourceView* hBlur2SRV = h2->GetRenderTargetSRV();

	// set horizontal blur render target 2
	h2->ClearRenderTarget(0, 0, 0, 1, false);
	h2->SetRendertarget(false, false);

	// change constants for pass 2
	constantVertex.horizontalPass = 1;
	constantVertex.screenWidth    = SystemSettings::SCREEN_WIDTH  / scaleDown2;
	constantVertex.screenHeight   = SystemSettings::SCREEN_HEIGHT / scaleDown2;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// get the horizontal blured texture from pass 1
	devCon->PSSetShaderResources(0, 1, &vBlur1SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	////////////////////////////////////////////////////////// VERTICAL PASS 2

	// set vertical blur render target 2
	v2->ClearRenderTarget(0, 0, 0, 1, false);
	v2->SetRendertarget(false, false);

	// change constants for vertical pass 2
	constantVertex.horizontalPass = 0;
	SHADER_HELPERS::UpdateConstantBuffer((void*)&constantVertex, sizeof(ConstantBlurVertex), _blurVertexConstant);

	// set horizontal blur texture
	devCon->PSSetShaderResources(0, 1, &hBlur2SRV);

	// draw vertical pass
	devCon->DrawIndexed(6, 0, 0);

	// return the final blurred image from vertical pass 2
	return v2->GetRenderTargetSRV();
}

void PostProcessingShader::RenderFinal(ID3D11ShaderResourceView* SceneImage, ID3D11ShaderResourceView* sceneDepth)
{
	// get dx manager
	DXManager& DXM = *Systems::dxManager;

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
	pixelConstant.applyDof       = PostProcessing::APPLY_DEPTH_OF_FIELD;
	pixelConstant.startEndDofdst = XMFLOAT2(PostProcessing::START_END_DOF_DST[0], PostProcessing::START_END_DOF_DST[1]);

	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixelConstant, sizeof(ConstantFinalPixel), _finalPixelConstant);

	// set textures
	ID3D11ShaderResourceView* texArray[4] = { SceneImage, _bloomMap, _dofMap, sceneDepth };
	devCon->PSSetShaderResources(0, 4, texArray);

	// draw
	devCon->DrawIndexed(6, 0, 0);
}

