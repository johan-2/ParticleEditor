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
#include "MathHelpers.h"
#include "Entity.h"
#include "JsonHelpers.h"

SkyDome::SkyDome(const char* settingsFile):
	_isActive(true)
{
	// create mesh and cubemap
	CreateMeshes();

	// create shaders
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeCubemap.vs",      _vertexDomeCubeMapShader,      _vertexDomeCubeMapShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeCubemap.ps",        _pixelDomeCubeMapShader,       _pixelDomeCubeMapShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeColorBlend.vs",   _vertexDomeColorBlendShader,   _vertexDomeColorBlendShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeColorBlend.ps",     _pixelDomeColorBlendShader,    _pixelDomeColorBlendShaderByteCode);
	SHADER_HELPERS::CreateVertexShader(L"shaders/SkyDome/vertexSkyDomeSun.vs",          _vertexSunShader,              _vertexSunShaderByteCode);
	SHADER_HELPERS::CreatePixelShader(L"shaders/SkyDome/pixelSkyDomeSun.ps",            _pixelSunShader,               _pixelSunShaderByteCode);

	// create constant buffers
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferVertex);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferColorBlendPixel);
	SHADER_HELPERS::CreateConstantBuffer(_constantBufferCubeMapBlendPixel);
	SHADER_HELPERS::CreateConstantBuffer(_constantSunPixel);

	// initialize sun/moon values
	/*_sunMoon.sun.distance           = XMFLOAT3(5.0f, 5.0f, 5.0f);
	_sunMoon.sun.beginEndFade       = XMFLOAT2(-0.1f, -0.25f);
	_sunMoon.sun.dayColorTint       = XMFLOAT3(1.0f, 1.0f, 1.0f);
	_sunMoon.sun.sunsetColorTint    = XMFLOAT3(0.95f, 0.65f, 0.1f);
	_sunMoon.sun.minMaxDst          = XMFLOAT2(0.8f, 5.0f);
	_sunMoon.sun.beginEndDstLerp    = XMFLOAT2(0.5f, -0.25f);
	_sunMoon.sun.beginEndColorBlend = XMFLOAT2(0.5f, -0.15f);

	_sunMoon.moon.distance           = XMFLOAT3(5.0f, 5.0f, 5.0f);
	_sunMoon.moon.beginEndFade       = XMFLOAT2(-0.1f, -0.25f);
	_sunMoon.moon.dayColorTint       = XMFLOAT3(1.0f, 1.0f, 1.0f);
	_sunMoon.moon.sunsetColorTint    = XMFLOAT3(0.5f, 0.5f, 0.5f);
	_sunMoon.moon.minMaxDst          = XMFLOAT2(8.0f, 10.0f);
	_sunMoon.moon.beginEndDstLerp    = XMFLOAT2(0.5f, -0.25f);
	_sunMoon.moon.beginEndColorBlend = XMFLOAT2(0.5f, -0.15f);*/
	
	_sunMoon.sun.entity = new Entity();	
 	_sunMoon.sun.transform = _sunMoon.sun.entity->AddComponent<TransformComponent>();
	_sunMoon.sun.transform->Init();

	_sunMoon.moon.entity = new Entity();
	_sunMoon.moon.transform = _sunMoon.moon.entity->AddComponent<TransformComponent>();
	_sunMoon.moon.transform->Init();

	ReadSettings(settingsFile);
	LoadCubemap();
}

SkyDome::~SkyDome()
{
	_vertexDomeCubeMapShaderByteCode->Release();
	_pixelDomeCubeMapShaderByteCode->Release();
	_vertexSunShaderByteCode->Release();
	_pixelSunShaderByteCode->Release();

	_vertexDomeCubeMapShader->Release();
	_pixelDomeCubeMapShader->Release();
	_vertexSunShader->Release();
	_pixelSunShader->Release();

	_constantBufferVertex->Release();
	_constantBufferColorBlendPixel->Release();
	_constantSunPixel->Release();
	_constantBufferCubeMapBlendPixel->Release();
}

void SkyDome::CreateMeshes() 
{
	_domeMesh          = ModelLoader::CreateSphere(0, L"", L"", L"", L"", 1.0f, nullptr, 0);
	_sunMoon.sun.mesh  = ModelLoader::CreateWorldSprite(0, L"Textures/sun.dds", nullptr);
	_sunMoon.moon.mesh = ModelLoader::CreateWorldSprite(0, L"Textures/moon2.dds", nullptr);
}

void SkyDome::LoadCubemap() 
{
	// remove the old cubemap texture if one exist
	if (_skySettings.cubeMap != nullptr)
		_skySettings.cubeMap->Release();

	std::wstring file(_skySettings.cubeMapName.begin(), _skySettings.cubeMapName.end());

	// create cubemap from file
	HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->device, file.c_str(), NULL, &_skySettings.cubeMap);

	// get and print error message if failed
	if (FAILED(result))						
		DX_ERROR::PrintError(result, (std::string("failed to create cubemap with filename ") + DX_ERROR::ConvertFromWString(file.c_str())).c_str());
}

void SkyDome::Render(bool noMask)
{
	if (!_isActive)
		return;

	if (_skySettings.RENDER_MODE == SKY_DOME_RENDER_MODE::CUBEMAP) RenderCubeMap(noMask);
	else                                                           RenderBlendedColors(noMask);
	
	RenderSunMoon(noMask);	
}

void SkyDome::RenderCubeMap(bool noMask)
{
	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext*& devCon = DXM.devCon;

	// get game camera
	CameraComponent*& camera     = Systems::cameraManager->currentCameraGame;
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// set shaders			
	devCon->VSSetShader(_vertexDomeCubeMapShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeCubeMapShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);

	// set texture
	devCon->PSSetShaderResources(0, 1, &_skySettings.cubeMap);

	// use no culling
	DXM.rasterizerStates->SetRasterizerState(RASTERIZER_STATE::NOCULL);

	if (noMask) DXM.depthStencilStates->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	else        DXM.depthStencilStates->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);
	
	CBVertDome vertexData;
	XMStoreFloat4x4(&vertexData.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&camTrans->positionMatrix, &camera->viewProjMatrix)));

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBVertDome), _constantBufferVertex);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();
	devCon->DrawIndexed(_domeMesh->numIndices, 0, 0);

	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	devCon->PSSetShaderResources(0, 1, nullSRV);
}

void SkyDome::RenderBlendedColors(bool noMask)
{
	// RENDER SKYDOME SPHERE
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.devCon;

	// get game camera
	CameraComponent* camera      = Systems::cameraManager->currentCameraGame;
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// set shaders			
	devCon->VSSetShader(_vertexDomeColorBlendShader, NULL, 0);
	devCon->PSSetShader(_pixelDomeColorBlendShader, NULL, 0);

	// set constant buffer
	devCon->VSSetConstantBuffers(0, 1, &_constantBufferVertex);
	devCon->PSSetConstantBuffers(0, 1, &_constantBufferColorBlendPixel);

	// use no culling
	DXM.rasterizerStates->SetRasterizerState(RASTERIZER_STATE::NOCULL);
	
	// set vertex constants
	if (noMask) DXM.depthStencilStates->SetDepthStencilState(DEPTH_STENCIL_STATE::DISABLED);
	else        DXM.depthStencilStates->SetDepthStencilState(DEPTH_STENCIL_STATE::MASKED_SKYBOX);
	
	CBVertDome vertexData;
	XMStoreFloat4x4(&vertexData.worldViewProj, XMLoadFloat4x4(&MATH_HELPERS::MatrixMutiplyTrans(&camTrans->positionMatrix, &camera->viewProjMatrix)));

	// set the colors, the percent fraction is stored in the w channel that specifies how
	// low to high a color will be used on the skydome 
	ConstantColorBlendPixel pixeldata;
	pixeldata.bottom   = _skySettings.skyBottomColor;
	pixeldata.mid      = _skySettings.skyMidColor;
	pixeldata.top      = _skySettings.skyTopColor;

	// update constant buffers
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBVertDome),              _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata,  sizeof(ConstantColorBlendPixel), _constantBufferColorBlendPixel);

	// upload and draw dome mesh
	_domeMesh->UploadBuffers();
	devCon->DrawIndexed(_domeMesh->numIndices, 0, 0);
}

void SkyDome::RenderSunMoon(bool noMask)
{
	// get DX manager
	DXManager& DXM = *Systems::dxManager;

	// get device context
	ID3D11DeviceContext* devCon = DXM.devCon;

	// get game camera
	CameraComponent* camera      = Systems::cameraManager->currentCameraGame;
	TransformComponent* camTrans = camera->GetComponent<TransformComponent>();

	// render sun with alpha blend
	DXM.blendStates->SetBlendState(BLEND_ALPHA);

	// set shaders			
	devCon->VSSetShader(_vertexSunShader, NULL, 0);
	devCon->PSSetShader(_pixelSunShader, NULL, 0);

	devCon->PSSetConstantBuffers(0, 1, &_constantSunPixel);

	// calculate sun/moon data
	CaluclateSunMoonMatrix(camTrans->position);

	// set world and projection matrix
	CBVertSun vertexData;
	XMStoreFloat4x4(&vertexData.world, XMLoadFloat4x4(&_sunMoon.sun.positionMatrix));
	XMStoreFloat4x4(&vertexData.view, XMMatrixTranspose(XMLoadFloat4x4(&camera->viewMatrix)));
	XMStoreFloat4x4(&vertexData.Proj, XMMatrixTranspose(XMLoadFloat4x4(&camera->projectionMatrix)));

	// constantbuffer pixel structure
	ConstantSunPixel pixeldata;
	pixeldata.sunDot       = _sunMoon.sun.relativeHeight;
	pixeldata.colorTint    = _sunMoon.sun.colorTint;
	pixeldata.beginEndFade = _sunMoon.sun.beginEndFade;

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBVertSun),        _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata,  sizeof(ConstantSunPixel), _constantSunPixel);

	// upload mesh
	_sunMoon.sun.mesh->UploadBuffers();

	// set texture
	devCon->PSSetShaderResources(0, 1, &_sunMoon.sun.mesh->baseTextures[0]);

	// upload and draw sun
	devCon->DrawIndexed(_sunMoon.sun.mesh->numIndices, 0, 0);

	// change properties for moon
	vertexData.world       = _sunMoon.moon.positionMatrix;
	pixeldata.sunDot       = _sunMoon.moon.relativeHeight;
	pixeldata.colorTint    = _sunMoon.moon.colorTint;
	pixeldata.beginEndFade = _sunMoon.moon.beginEndFade;

	// update constant buffer
	SHADER_HELPERS::UpdateConstantBuffer((void*)&vertexData, sizeof(CBVertSun),       _constantBufferVertex);
	SHADER_HELPERS::UpdateConstantBuffer((void*)&pixeldata, sizeof(ConstantSunPixel), _constantSunPixel);

	// upload mesh
	_sunMoon.moon.mesh->UploadBuffers();

	// set texture
	devCon->PSSetShaderResources(0, 1, &_sunMoon.moon.mesh->baseTextures[0]);

	// upload and draw moon
	devCon->DrawIndexed(_sunMoon.moon.mesh->numIndices, 0, 0);

	// enable rendering of all pixels
	DXM.depthStencilStates->SetDepthStencilState(DEPTH_STENCIL_STATE::ENABLED);

	// set back to backcull
	DXM.rasterizerStates->SetRasterizerState(RASTERIZER_STATE::BACKCULL);

	ID3D11ShaderResourceView* nullSRV[1] = { NULL };
	devCon->PSSetShaderResources(0, 1, nullSRV);
}

void SkyDome::CaluclateSunMoonMatrix(XMFLOAT3 cameraPosition)
{
	XMFLOAT3 offsetSun;
	XMFLOAT3 offsetMoon;
	XMFLOAT3 sunDirection  = _sunMoon.sun.transform->GetForward();
	XMFLOAT3 moonDirection = _sunMoon.moon.transform->GetForward();
	
	// calculate translation for sun and moon
	XMStoreFloat3(&offsetSun,  XMVectorMultiply(XMLoadFloat3(&sunDirection), XMLoadFloat3(&_sunMoon.sun.distance)));
	XMStoreFloat3(&offsetMoon, XMVectorMultiply(XMLoadFloat3(&moonDirection), XMLoadFloat3(&_sunMoon.moon.distance)));

	XMStoreFloat3(&offsetSun,  XMVectorSubtract(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&offsetSun)));
	XMStoreFloat3(&offsetMoon, XMVectorSubtract(XMLoadFloat3(&cameraPosition), XMLoadFloat3(&offsetMoon)));

	// calculate position matrix for sun and moon
	XMStoreFloat4x4(&_sunMoon.sun.positionMatrix,  XMMatrixTranslationFromVector(XMLoadFloat3(&offsetSun)));
	XMStoreFloat4x4(&_sunMoon.sun.positionMatrix,  XMMatrixTranspose(XMLoadFloat4x4(&_sunMoon.sun.positionMatrix)));

	XMStoreFloat4x4(&_sunMoon.moon.positionMatrix, XMMatrixTranslationFromVector(XMLoadFloat3(&offsetMoon)));
	XMStoreFloat4x4(&_sunMoon.moon.positionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&_sunMoon.moon.positionMatrix)));
}

void SkyDome::Update(const float& delta)
{
	UpdateSunMoonTranslation(delta);
	UpdateSunMoonColors(delta);
	UpdateShadowLightTranslation(delta);
	UpdateShadowLightColor(delta);

	if (_skySettings.RENDER_MODE == THREE_LAYER_COLOR_BLEND) UpdateSkyColors(delta);
}

void SkyDome::UpdateSunMoonTranslation(const float& delta)
{
	// add to cycle timer and reset if we have passed one complete cycle
	_skySettings.cycleTimer += delta * _skySettings.speedMultiplier;
	if (_skySettings.cycleTimer > _skySettings.cycleInSec)
		_skySettings.cycleTimer = 0.0f;

	XMFLOAT3 sunRotation;
	XMFLOAT3 moonRotation;
	XMFLOAT3 moonOffset(180.0f, 0, 0);

	// get the current rotation of the sun bassed on cycle passed
	_skySettings.endRotation.y = _skySettings.startRotation.y;
	XMStoreFloat3(&sunRotation,
		XMVectorLerp(XMLoadFloat3(&_skySettings.startRotation),
			XMLoadFloat3(&_skySettings.endRotation),
			_skySettings.cycleTimer / _skySettings.cycleInSec));

	// add on 180 degress to moon rotation from sun rotation
	XMStoreFloat3(&moonRotation, XMVectorAdd(XMLoadFloat3(&sunRotation), XMLoadFloat3(&moonOffset)));

	// update rotation and matrices
	_sunMoon.sun.transform->rotation  = sunRotation;
	_sunMoon.moon.transform->rotation = moonRotation;
	_sunMoon.sun.transform->BuildWorldMatrix();
	_sunMoon.moon.transform->BuildWorldMatrix();

	// get the inverse look of sun
	XMFLOAT3 invertedForward = _sunMoon.sun.transform->GetForward();
	XMStoreFloat3(&invertedForward, XMVectorNegate(XMLoadFloat3(&invertedForward)));

	// get the dot product that will represent the sun at its highest point
	// 1.0 = top, 0.0 = at horizon, -1 = at lowest point under the world
	XMFLOAT3 up(0, 1, 0);
	XMStoreFloat(&_sunMoon.sun.relativeHeight,
		XMVector3Dot(XMLoadFloat3(&invertedForward), XMLoadFloat3(&up)));

	_sunMoon.moon.relativeHeight = -_sunMoon.sun.relativeHeight;

	// set distance of sun/moon depending on thier respective relative height
	float dst = lerpF(_sunMoon.sun.minMaxDst.y, _sunMoon.sun.minMaxDst.x,
		inverseLerp(_sunMoon.sun.beginEndDstLerp.x, _sunMoon.sun.beginEndDstLerp.y, _sunMoon.sun.relativeHeight));
	_sunMoon.sun.distance = XMFLOAT3(dst, dst, dst);

	dst = lerpF(_sunMoon.moon.minMaxDst.y, _sunMoon.moon.minMaxDst.x,
		inverseLerp(_sunMoon.moon.beginEndDstLerp.x, _sunMoon.moon.beginEndDstLerp.y, _sunMoon.moon.relativeHeight));
	_sunMoon.moon.distance = XMFLOAT3(dst, dst, dst);
}

void SkyDome::UpdateSunMoonColors(const float& delta)
{
	// set color tint on sun/moon depending on thier own relative height
	LerpColorRGB(_sunMoon.sun.colorTint, _sunMoon.sun.dayColorTint, _sunMoon.sun.sunsetColorTint,
		_sunMoon.sun.beginEndColorBlend.x, _sunMoon.sun.beginEndColorBlend.y, _sunMoon.sun.relativeHeight);

	LerpColorRGB(_sunMoon.moon.colorTint, _sunMoon.moon.dayColorTint, _sunMoon.moon.sunsetColorTint,
		_sunMoon.moon.beginEndColorBlend.x, _sunMoon.moon.beginEndColorBlend.y, _sunMoon.moon.relativeHeight);
}

void SkyDome::UpdateShadowLightTranslation(const float& delta)
{
	// get the transform of the camera that renders the shadowmap, the directional light 
	// use this same transform for setting the light direction
	TransformComponent* lightTransform = Systems::cameraManager->currentCameraDepthMap->GetComponent<TransformComponent>();

	// set new light rotation depending on the threshold if
	// the sun or the moon is the light casting source
	lightTransform->rotation = _sunMoon.sun.relativeHeight < _skySettings.switchToMoonLightThreshold ? _sunMoon.moon.transform->rotation : _sunMoon.sun.transform->rotation;
	lightTransform->BuildWorldMatrix();

	// get the inverted forward of the current lightsource
	// so we can offset the shadowmap camera based on this
	XMFLOAT3 invertedForwardLight;
	XMStoreFloat3(&invertedForwardLight, XMVectorNegate(XMLoadFloat3(&lightTransform->GetForward())));

	// move the shadowmap camera 
	XMStoreFloat3(&lightTransform->position, XMVectorMultiply(XMLoadFloat3(&invertedForwardLight), XMLoadFloat3(&_skySettings.shadowMapDistance)));
	lightTransform->BuildWorldMatrix();
}

void SkyDome::UpdateShadowLightColor(const float& delta)
{
	// get the directional light so we can change the colors of light
	LightDirectionComponent*& directionLight = Systems::lightManager->directionalLight;

	// final directional light color depending on the dot product
	// day to sunset
	XMFLOAT4 blendedLightColor;
	LerpColorRGB(blendedLightColor, _skySettings.normalDirLightColor, _skySettings.sunsetDirLightColor,
		_skySettings.sunsetLightColorStartEndBlend.x, _skySettings.sunsetLightColorStartEndBlend.y, _sunMoon.sun.relativeHeight);

	// sunset to night
	LerpColorRGB(blendedLightColor, blendedLightColor, _skySettings.nightDirLightColor,
		_skySettings.nightLightColorStartEndBlend.x, _skySettings.nightLightColorStartEndBlend.y, _sunMoon.sun.relativeHeight);

	// get the light strength fraction depending on if night or day
	// this makes the light completely fade out before we switch between
	// the sun and moon light sources. After the switch the light strength is 
	// then faded back in, this avoids a rough light transition
	float ls = _sunMoon.sun.relativeHeight < _skySettings.switchToMoonLightThreshold ?
		inverseLerp(_skySettings.nightLightStartEndfade.y, _skySettings.nightLightStartEndfade.x, _sunMoon.sun.relativeHeight) :
		inverseLerp(_skySettings.dayLightStartEndfade.y,   _skySettings.dayLightStartEndfade.x,   _sunMoon.sun.relativeHeight);

	// mutiply light color by strenght
	XMFLOAT4 lightStrength(ls, ls, ls, 1.0f);
	XMStoreFloat4(&blendedLightColor, XMVectorMultiply(XMLoadFloat4(&blendedLightColor), XMLoadFloat4(&lightStrength)));

	// set the color of directional light
	directionLight->lightColor = blendedLightColor;
}

void SkyDome::UpdateSkyColors(const float& delta)
{
	// set the color of the top part of sky
	// blend between day color to sunsetcolor and then to nightcolor
	XMFLOAT4 topSkyColor;
	LerpColorRGB(topSkyColor, _skySettings.topSkyColorDay, _skySettings.topSkyColorSunSet,
		_skySettings.sunsetTopSkyColorStartEndBlend.x,     _skySettings.sunsetTopSkyColorStartEndBlend.y, _sunMoon.sun.relativeHeight);

	LerpColorRGB(_skySettings.skyTopColor, topSkyColor, _skySettings.topSkyColorNight,
		_skySettings.nightTopSkyColorStartEndBlend.x,    _skySettings.nightTopSkyColorStartEndBlend.y, _sunMoon.sun.relativeHeight);

	// set the color of the mid part of sky
	// blend between day color to sunsetcolor and then to nightcolor
	XMFLOAT4 midSkyColor;
	LerpColorRGB(midSkyColor, _skySettings.midSkyColorDay, _skySettings.midSkyColorSunSet,
		_skySettings.sunsetMidSkyColorStartEndBlend.x, _skySettings.sunsetMidSkyColorStartEndBlend.y, _sunMoon.sun.relativeHeight);

	LerpColorRGB(_skySettings.skyMidColor, midSkyColor, _skySettings.midSkyColorNight,
		_skySettings.nightMidSkyColorStartEndBlend.x, _skySettings.nightMidSkyColorStartEndBlend.y, _sunMoon.sun.relativeHeight);
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

void SkyDome::ReadSettings(const char* file)
{
	FILE* fp; fopen_s(&fp, file, "rb");
	char readBuffer[65536];
	rapidjson::FileReadStream inStream(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(inStream);
	fclose(fp);

	assert(d.IsObject());

	_skySettings.speedMultiplier            = JSON::ReadFloat(d, "speedMultiplier");
	_skySettings.cycleTimer                 = JSON::ReadFloat(d, "cycleTimer");
	_skySettings.switchToMoonLightThreshold = JSON::ReadFloat(d, "switchToMoonLightThreshold");

	_skySettings.sunsetLightColorStartEndBlend  = JSON::ReadFloat2(d, "sunsetLightColorStartEndBlend");
	_skySettings.nightLightColorStartEndBlend   = JSON::ReadFloat2(d, "nightLightColorStartEndBlend");
	_skySettings.sunsetTopSkyColorStartEndBlend = JSON::ReadFloat2(d, "sunsetTopSkyColorStartEndBlend");
	_skySettings.nightTopSkyColorStartEndBlend  = JSON::ReadFloat2(d, "nightTopSkyColorStartEndBlend");
	_skySettings.sunsetMidSkyColorStartEndBlend = JSON::ReadFloat2(d, "sunsetMidSkyColorStartEndBlend");
	_skySettings.nightMidSkyColorStartEndBlend  = JSON::ReadFloat2(d, "nightMidSkyColorStartEndBlend");

	_skySettings.dayLightStartEndfade   = JSON::ReadFloat2(d, "dayLightStartEndfade");
	_skySettings.nightLightStartEndfade = JSON::ReadFloat2(d, "nightLightStartEndfade");

	_skySettings.shadowMapDistance = JSON::ReadFloat3(d, "shadowMapDistance");
	_skySettings.startRotation     = JSON::ReadFloat3(d, "startRotation");
	_skySettings.endRotation       = JSON::ReadFloat3(d, "endRotation");

	_skySettings.normalDirLightColor = JSON::ReadFloat4(d, "normalDirLightColor");
	_skySettings.sunsetDirLightColor = JSON::ReadFloat4(d, "sunsetDirLightColor");
	_skySettings.nightDirLightColor  = JSON::ReadFloat4(d, "nightDirLightColor");
	_skySettings.topSkyColorDay      = JSON::ReadFloat4(d, "topSkyColorDay");
	_skySettings.topSkyColorSunSet   = JSON::ReadFloat4(d, "topSkyColorSunSet");
	_skySettings.topSkyColorNight    = JSON::ReadFloat4(d, "topSkyColorNight");
	_skySettings.midSkyColorDay      = JSON::ReadFloat4(d, "midSkyColorDay");
	_skySettings.midSkyColorSunSet   = JSON::ReadFloat4(d, "midSkyColorSunSet");
	_skySettings.midSkyColorNight    = JSON::ReadFloat4(d, "midSkyColorNight");
	_skySettings.skyBottomColor      = JSON::ReadFloat4(d, "skyBottomColor");

	_skySettings.RENDER_MODE = (SKY_DOME_RENDER_MODE)JSON::ReadInt(d, "RENDER_MODE");
	_skySettings.cubeMapName = JSON::ReadString(d, "cubeMap");

	_sunMoon.sun.dayColorTint     = JSON::ReadFloat3(d, "sunDayColorTint");
	_sunMoon.moon.dayColorTint    = JSON::ReadFloat3(d, "moonDayColorTint");
	_sunMoon.sun.sunsetColorTint  = JSON::ReadFloat3(d, "sunSunsetColorTint");
	_sunMoon.moon.sunsetColorTint = JSON::ReadFloat3(d, "moonSunsetColorTint");

	_sunMoon.sun.beginEndFade  = JSON::ReadFloat2(d, "sunBeginEndfade");
	_sunMoon.moon.beginEndFade = JSON::ReadFloat2(d, "moonBeginEndfade");

	_sunMoon.sun.minMaxDst  = JSON::ReadFloat2(d, "sunMinMaxDst");
	_sunMoon.moon.minMaxDst = JSON::ReadFloat2(d, "moonMinMaxDst");

	_sunMoon.sun.beginEndDstLerp  = JSON::ReadFloat2(d, "sunBeginEndDstLerp");
	_sunMoon.moon.beginEndDstLerp = JSON::ReadFloat2(d, "moonBeginEndDstLerp");

	_sunMoon.sun.beginEndColorBlend  = JSON::ReadFloat2(d, "sunBeginEndColorBlend");
	_sunMoon.moon.beginEndColorBlend = JSON::ReadFloat2(d, "moonBeginEndColorBlend");
}