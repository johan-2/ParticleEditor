#include "SkyDome.h"
#include "CameraManager.h"
#include "DXManager.h"
#include "TransformComponent.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include "DXErrorHandler.h"
#include "ShaderHelpers.h"
#include "DXBlendstates.h"
#include "DXRasterizerStates.h"
#include "DXDepthStencilStates.h"
#include "Systems.h"
#include "ModelLoader.h"
#include "Mesh.h"
#include <algorithm>

SkyDome::SkyDome(const wchar_t* textureFile, SKY_DOME_RENDER_MODE mode):
	_isActive(true),
	_RENDER_MODE(mode)
{
	// create mesh and cubemap
	CreateMeshes();
	LoadCubemap(textureFile);

	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeCubemap.vs",      _vertexDomeCubeMapShader,      _vertexDomeCubeMapShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeCubemap.ps",        _pixelDomeCubeMapShader,       _pixelDomeCubeMapShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeColorBlend.vs",   _vertexDomeColorBlendShader,   _vertexDomeColorBlendShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeColorBlend.ps",     _pixelDomeColorBlendShader,    _pixelDomeColorBlendShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeCubemapBlend.vs", _vertexDomeCubeMapBlendShader, _vertexDomeCubeMapBlendShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeCubemapBlend.ps",   _pixelDomeCubeMapBlendShader,  _pixelDomeCubeMapBlendShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeSun.vs",          _vertexSunShader,              _vertexSunShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeSun.ps",            _pixelSunShader,               _pixelSunShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferColorBlendPixel);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferCubeMapBlendPixel);
	SHADER_HELPERS::CreateConstantBuffer(_constantSunPixel);

	// initialize sun/moon values
	_sunMoon.sun.distance      = XMFLOAT3(5.0f, 5.0f, 5.0f);
	_sunMoon.sun.beginEndFade  = XMFLOAT2(-0.1f, -0.25f);
	_sunMoon.moon.distance     = XMFLOAT3(5.0f, 5.0f, 5.0f);
	_sunMoon.moon.beginEndFade = XMFLOAT2(0.3f, 0.05f);
	_sunMoon.moon.colorTint    = XMFLOAT3(0.4f, 0.4f, 0.4f);
	_sunMoon.directionPtr      = nullptr;
}

SkyDome::~SkyDome()
{
	_vertexDomeCubeMapShaderByteCode->Release();
	_pixelDomeCubeMapShaderByteCode->Release();
	_vertexSunShaderByteCode->Release();
	_pixelSunShaderByteCode->Release();
	_vertexDomeCubeMapBlendShaderByteCode->Release();
	_pixelDomeCubeMapBlendShaderByteCode->Release();

	_vertexDomeCubeMapShader->Release();
	_pixelDomeCubeMapShader->Release();
	_vertexSunShader->Release();
	_pixelSunShader->Release();
	_vertexDomeCubeMapBlendShader->Release();
	_pixelDomeCubeMapBlendShader->Release();

	_constantBufferVertex->Release();
	_constantBufferColorBlendPixel->Release();
	_constantSunPixel->Release();
	_constantBufferCubeMapBlendPixel->Release();
}

void SkyDome::CreateMeshes() 
{
	_domeMesh          = ModelLoader::CreateSphere(0, L"", L"", L"", L"", 1.0f, nullptr);
	_sunMoon.sun.mesh  = ModelLoader::CreateWorldSprite(0, L"Textures/sun.dds", nullptr);
	_sunMoon.moon.mesh = ModelLoader::CreateWorldSprite(0, L"Textures/moon.dds", nullptr);
}

void SkyDome::LoadCubemap(const wchar_t* file) 
{
	// remove the old cubemap texture if one exist
	if (_cubeMap != nullptr)
		_cubeMap->Release();

	// create cubemap from file
	HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->GetDevice(), file, NULL, &_cubeMap);

	// get and print error message if failed
	if (FAILED(result))						
		DX_ERROR::PrintError(result, (std::string("failed to create cubemap with filename ") + DX_ERROR::ConvertFromWString(file)).c_str());
}

void SkyDome::Render(bool useReflectViewMatrix)
{
	if (!_isActive)
		return;

	if      (_RENDER_MODE == SKY_DOME_RENDER_MODE::CUBEMAP_SIMPLE)          RenderCubeMapSimple(useReflectViewMatrix);
	else if (_RENDER_MODE == SKY_DOME_RENDER_MODE::THREE_LAYER_COLOR_BLEND) RenderBlendedColors(useReflectViewMatrix);
	else if (_RENDER_MODE == SKY_DOME_RENDER_MODE::CUBEMAP_COLOR_BLEND)     RenderCubeMapColorBlend(useReflectViewMatrix);
	
	RenderSunMoon(useReflectViewMatrix);	
}

void SkyDome::RenderCubeMapSimple(bool useReflectViewMatrix)
{
	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent*& camera     = Systems::cameraManager->GetCurrentCameraGame();
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// set shaders			
	devCon->VSSetShader(_vertexDomeCubeMapShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeCubeMapShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_cubeMap);

	// use no culling
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	// only render non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);

	// constantbuffer structure
	ConstantVertex vertexData;

	// set the matrices
	XMStoreFloat4x4(&vertexData.world, XMMatrixTranspose(XMLoadFloat4x4(&camTrans->GetPositionMatrix())));
	XMStoreFloat4x4(&vertexData.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));
	if (useReflectViewMatrix)
	{
		XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetReflectionViewMatrix(camTrans->GetPositionRef().y)));
		DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	}
	else XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetViewMatrix()));

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();
	devCon->DrawIndexed(_domeMesh->GetNumIndices(), 0, 0);
}

void SkyDome::RenderBlendedColors(bool useReflectViewMatrix)
{
	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent* camera      = Systems::cameraManager->GetCurrentCameraGame();
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// set shaders			
	devCon->VSSetShader(_vertexDomeColorBlendShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeColorBlendShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferColorBlendPixel);

	// use no culling
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	// only render non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);
	
	// set vertex constants
	ConstantVertex vertexData;
	XMStoreFloat4x4(&vertexData.world, XMMatrixTranspose(XMLoadFloat4x4(&camTrans->GetPositionMatrix())));
	XMStoreFloat4x4(&vertexData.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));
	if (useReflectViewMatrix) 
	{
		XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetReflectionViewMatrix(camTrans->GetPositionRef().y))); 
		DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	}
	else XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetViewMatrix()));

	// set the colors, the percent fraction is stored in the w channel that specifies how
	// low to high a color will be used on the skydome 
	ConstantColorBlendPixel pixeldata;
	pixeldata.bottom   = _skyColorLayers.bottomColor;
	pixeldata.mid      = _skyColorLayers.midColor;
	pixeldata.top      = _skyColorLayers.topColor;

	// update constant buffers
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex),          _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata,  sizeof(ConstantColorBlendPixel), _constantBufferColorBlendPixel);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();
	devCon->DrawIndexed(_domeMesh->GetNumIndices(), 0, 0);
}

void SkyDome::RenderCubeMapColorBlend(bool useReflectViewMatrix) 
{
	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent* camera      = Systems::cameraManager->GetCurrentCameraGame();
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// set shaders			
	devCon->VSSetShader(_vertexDomeCubeMapBlendShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeCubeMapBlendShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferCubeMapBlendPixel);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_cubeMap);

	// use no culling
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	// only render non geometry pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);

	// set vertex constants
	ConstantVertex vertexData;
	XMStoreFloat4x4(&vertexData.world, XMMatrixTranspose(XMLoadFloat4x4(&camTrans->GetPositionMatrix())));
	XMStoreFloat4x4(&vertexData.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));
	if (useReflectViewMatrix)
	{
		XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetReflectionViewMatrix(camTrans->GetPositionRef().y)));
		DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	}
	else XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetViewMatrix()));

	// set the colors, the percent fraction is stored in the w channel that specifies how
	// low to high a color will be used on the skydome 
	ConstantCubeMapColorBlendPixel pixelData;
	pixelData.bottomBlend    = _skyColorLayers.bottomColor.w;
	pixelData.midBlend       = _skyColorLayers.midColor.w;
	pixelData.topBlend       = _skyColorLayers.topColor.w;
	pixelData.cubeMapIsTop   = _cubeMapColorBlend.topIsCubeMap;
	pixelData.topBottomColor = _cubeMapColorBlend.topIsCubeMap ? _skyColorLayers.bottomColor : _skyColorLayers.topColor;
	pixelData.midColor       = _skyColorLayers.midColor;

	// update constant buffers
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex),                 _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixelData,  sizeof(ConstantCubeMapColorBlendPixel), _constantBufferCubeMapBlendPixel);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();
	devCon->DrawIndexed(_domeMesh->GetNumIndices(), 0, 0);
}

void SkyDome::RenderSunMoon(bool reflect)
{
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.GetDeviceCon();

	// get game camera
	CameraComponent* camera             = Systems::cameraManager->GetCurrentCameraGame();
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// render sun with alpha blend
	DXM.BlendStates()->SetBlendState(BLEND_ALPHA);

	// set shaders			
	devCon->VSSetShader(_vertexSunShader, NULL, 0);
	devCon->PSSetShader(_pixelSunShader, NULL, 0);

	devCon->PSSetConstantBuffers(0, 1, &_constantSunPixel);

	// calculate sun/moon data
	CaluclateSunMoonMatrix(camTrans->GetPositionRef());

	// set vertex constants
	ConstantVertex vertexData;
	XMStoreFloat4x4(&vertexData.world, XMLoadFloat4x4(&_sunMoon.sun.positionMatrix));
	XMStoreFloat4x4(&vertexData.projection, XMLoadFloat4x4(&camera->GetProjectionMatrix()));
	if (reflect)
	{
		XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetReflectionViewMatrix(camTrans->GetPositionRef().y)));
		DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	}
	else XMStoreFloat4x4(&vertexData.view, XMLoadFloat4x4(&camera->GetViewMatrix()));

	// constantbuffer pixel structure
	ConstantSunPixel pixeldata;
	pixeldata.sunDot       = _sunMoon.sun.relativeHeight;
	pixeldata.colorTint    = _sunMoon.sun.colorTint;
	pixeldata.beginEndFade = _sunMoon.sun.beginEndFade;

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex),   _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata,  sizeof(ConstantSunPixel), _constantSunPixel);

	// upload mesh
	_sunMoon.sun.mesh->UploadBuffers();

	// set texture
	devCon->PSSetShaderResources(0, 1, &_sunMoon.sun.mesh->GetTextureArray()[0]);

	// upload and draw sun
	devCon->DrawIndexed(_sunMoon.sun.mesh->GetNumIndices(), 0, 0);

	// change properties for moon
	vertexData.world       = _sunMoon.moon.positionMatrix;
	pixeldata.sunDot       = _sunMoon.moon.relativeHeight;
	pixeldata.colorTint    = _sunMoon.moon.colorTint;
	pixeldata.beginEndFade = _sunMoon.moon.beginEndFade;

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(ConstantVertex), _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata, sizeof(ConstantSunPixel), _constantSunPixel);

	// upload mesh
	_sunMoon.moon.mesh->UploadBuffers();

	// set texture
	devCon->PSSetShaderResources(0, 1, &_sunMoon.moon.mesh->GetTextureArray()[0]);

	// upload and draw moon
	devCon->DrawIndexed(_sunMoon.moon.mesh->GetNumIndices(), 0, 0);

	// enable rendering of all pixels
	DXM.DepthStencilStates()->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);

	// set back to backcull
	DXM.RasterizerStates()->SetRasterizerState(RASTERIZER_STATE::BACKCULL);
}

void SkyDome::CaluclateSunMoonMatrix(XMFLOAT3 cameraPosition)
{
	XMFLOAT3 offsetSun;
	XMFLOAT3 offsetMoon;
	XMFLOAT3 sunDirection = _sunMoon.directionPtr->GetForward();
	
	// calculate translation for sun and moon
	XMStoreFloat3(&offsetSun,  XMVectorMultiply(XMLoadFloat3(&sunDirection), XMLoadFloat3(&_sunMoon.sun.distance)));
	XMStoreFloat3(&offsetMoon, XMVectorMultiply(XMLoadFloat3(&sunDirection), XMLoadFloat3(&_sunMoon.moon.distance)));

	XMStoreFloat3(&offsetSun, XMVectorSubtract(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&offsetSun)));
	XMStoreFloat3(&offsetMoon, XMVectorAdd(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&offsetMoon)));

	// calculate position matrix for sun and moon
	XMStoreFloat4x4(&_sunMoon.sun.positionMatrix,  XMMatrixTranslationFromVector(XMLoadFloat3(&offsetSun)));
	XMStoreFloat4x4(&_sunMoon.sun.positionMatrix,  XMMatrixTranspose(XMLoadFloat4x4(&_sunMoon.sun.positionMatrix)));

	XMStoreFloat4x4(&_sunMoon.moon.positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&offsetMoon)));
	XMStoreFloat4x4(&_sunMoon.moon.positionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&_sunMoon.moon.positionMatrix)));

	// get the dot product of sun from world up vector and inversed sun direction
	// 1 = sun at its highest point, -1 sun at its lowest point
	XMFLOAT3 vec1;
	XMFLOAT3 vec2(0, 1, 0);
	XMStoreFloat3(&vec1, XMVectorNegate(XMLoadFloat3(&sunDirection)));
	XMStoreFloat(&_sunMoon.sun.relativeHeight,
		XMVector3Dot(XMLoadFloat3(&vec1), XMLoadFloat3(&vec2)));

	// the moon is just the opposite dot of sun
	_sunMoon.moon.relativeHeight = -_sunMoon.sun.relativeHeight;
}

void SkyDome::Update(const float& delta)
{
	if (_dynamicSky.isEnabled)
		UpdateDynamicSky(delta);
}

void SkyDome::UpdateDynamicSky(const float& delta)
{
	// get the transform that controls the light direction and the lightdirection itself
	TransformComponent*      lightTransform = Systems::cameraManager->GetCurrentCameraDepthMap()->GetComponent<TransformComponent>();
	LightDirectionComponent* directionLight = Systems::lightManager->GetDirectionalLight();

	// add to cycle timer and reset if we have passed one complete cycle
	_dynamicSky.cycleTimer += delta * _dynamicSky.speedMultiplier;
	if (_dynamicSky.cycleTimer > _dynamicSky.cycleInSec)
		_dynamicSky.cycleTimer = 0.0f;

	// get the current rotation of light depending on fraction of cycle passed
	XMStoreFloat3(&_dynamicSky.rotation,
		XMVectorLerp(XMLoadFloat3(&_dynamicSky.startRotation),
			XMLoadFloat3(&_dynamicSky.endRotation),
			_dynamicSky.cycleTimer / _dynamicSky.cycleInSec));

	// set new light rotation and update the world matrix
	lightTransform->SetRotation(_dynamicSky.rotation);
	lightTransform->UpdateWorldMatrix();

	// get the inverse look at of light so we can translate back from origin
	XMFLOAT3 invertedForward = lightTransform->GetForward();
	XMStoreFloat3(&invertedForward, XMVectorNegate(XMLoadFloat3(&invertedForward)));

	// get and set the final position of light source
	XMFLOAT3 finalPosition;
	XMStoreFloat3(&finalPosition, XMVectorMultiply(XMLoadFloat3(&invertedForward), XMLoadFloat3(&_dynamicSky.shadowMapDistance)));
	lightTransform->SetPosition(finalPosition);
	lightTransform->UpdateWorldMatrix();

	// get the dot product that will represent the sun at its highest point
	// 1.0 = top, 0.0 = at horizon, -1 = at lowest point under the world
	XMFLOAT3 up(0, 1, 0);
	float highestPoint;
	XMStoreFloat(&highestPoint,
		XMVector3Dot(XMLoadFloat3(&invertedForward), XMLoadFloat3(&up)));

	// make sun distance smaller the closer to the horizon it gets
	// this will make it appear bigger
	float dst = lerpF(_dynamicSky.sunMinMaxDst.y, _dynamicSky.sunMinMaxDst.x,
		inverseLerp(_dynamicSky.sunBeginEndDstLerp.x, _dynamicSky.sunBeginEndDstLerp.y, highestPoint));
	_sunMoon.sun.distance = XMFLOAT3(dst, dst, dst);

	// set color tint on sun depending on sundown
	LerpColorRGB(_sunMoon.sun.colorTint, _dynamicSky.sunDayColorTint, _dynamicSky.sunSunsetColorTint,
		_dynamicSky.sunBeginEndColorBlend.x, _dynamicSky.sunBeginEndColorBlend.y, highestPoint);

	// final directional light color depending on the dot product
	XMFLOAT4 blendedLightColor;
	LerpColorRGB(blendedLightColor, _dynamicSky.normalDirLightColor, _dynamicSky.sunsetDirLightColor,
		_dynamicSky.sunsetLightColorStartEndBlend.x, _dynamicSky.sunsetLightColorStartEndBlend.y, highestPoint);

	// set the strength of directional light based on sun height
	float ls = inverseLerp(_dynamicSky.sunLightBeginEndFade.y, _dynamicSky.sunLightBeginEndFade.x, highestPoint);
	XMFLOAT4 lightStrength(ls, ls, ls, 1.0f);
	XMStoreFloat4(&blendedLightColor, XMVectorMultiply(XMLoadFloat4(&blendedLightColor), XMLoadFloat4(&lightStrength)));
	directionLight->SetLightColor(blendedLightColor);

	// set the color of the top part of sky
	// blend between day color to sunsetcolor and then to nightcolor
	XMFLOAT4 topSkyColor;
	LerpColorRGB(topSkyColor, _dynamicSky.topSkyColorDay, _dynamicSky.topSkyColorSunSet,
		_dynamicSky.sunsetTopSkyColorStartEndBlend.x, _dynamicSky.sunsetTopSkyColorStartEndBlend.y, highestPoint);

	LerpColorRGB(_skyColorLayers.topColor, topSkyColor, _dynamicSky.topSkyColorNight,
		_dynamicSky.nightTopSkyColorStartEndBlend.x, _dynamicSky.nightTopSkyColorStartEndBlend.y, highestPoint);

	// set the color of the mid part of sky
	// blend between day color to sunsetcolor and then to nightcolor
	XMFLOAT4 midSkyColor;
	LerpColorRGB(midSkyColor, _dynamicSky.midSkyColorDay, _dynamicSky.midSkyColorSunSet,
		_dynamicSky.sunsetMidSkyColorStartEndBlend.x, _dynamicSky.sunsetMidSkyColorStartEndBlend.y, highestPoint);

	LerpColorRGB(_skyColorLayers.midColor, midSkyColor, _dynamicSky.midSkyColorNight,
		_dynamicSky.nightMidSkyColorStartEndBlend.x, _dynamicSky.nightMidSkyColorStartEndBlend.y, highestPoint);
}

float SkyDome::inverseLerp(float a, float b, float t)
{
	float result = ((t - a) / (b - a));

	return result < 0.0f ? 0.0f : result > 1.0f ? 1.0f : result;	
}

float SkyDome::lerpF(float a, float b, float f)
{
	return (a * (1.0f - f)) + (b * f);
}

void SkyDome::LerpColorRGB(XMFLOAT4& result, XMFLOAT4 colorA, XMFLOAT4 colorB, float startBlend, float endBlend, float fraction)
{
	XMFLOAT4 color;

	XMStoreFloat4(&color,
		XMVectorLerp(XMLoadFloat4(&colorA), XMLoadFloat4(&colorB), inverseLerp(startBlend, endBlend, fraction)));

	result.x = color.x;
	result.y = color.y;
	result.z = color.z;
}

void SkyDome::LerpColorRGB(XMFLOAT3& result, XMFLOAT3 colorA, XMFLOAT3 colorB, float startBlend, float endBlend, float fraction)
{
	XMStoreFloat3(&result,
		XMVectorLerp(XMLoadFloat3(&colorA), XMLoadFloat3(&colorB), inverseLerp(startBlend, endBlend, fraction)));
}