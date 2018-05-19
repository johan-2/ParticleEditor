#include "DXManager.h"
#include <iostream>

#include "SystemDefs.h"

DXManager* DXManager::_instance = 0;

DXManager& DXManager::GetInstance()
{
	if (_instance == 0)
		_instance = new DXManager;

	return *_instance;
}

DXManager::DXManager()
{
	_swapChain = 0;
	_device = 0;
	_devCon = 0;
	_renderTargetView = 0;
	_depthStencilEnabled = 0;
	_depthStencilView = 0;
	_rasterizerBackCull = 0;
	_blendStateAlpha = 0;
	_blendStateOpaque = 0;
	_depthStencilDisabled = 0;
	_blendStateAlpha = 0;
	_rasterizerWireframe = 0;
	
}


DXManager::~DXManager()
{
	
}

void DXManager::Shutdown() 
{

	if (_swapChain) 
		_swapChain->SetFullscreenState(false, NULL);
	
	if (_device) 
		_device->Release();
	
	if (_devCon) 
		_devCon->Release();
	
	if (_renderTargetView) 
		_renderTargetView->Release();		
	
	if (_depthStencilEnabled) 
		_depthStencilEnabled->Release();
	
	if (_rasterizerBackCull) 
		_rasterizerBackCull->Release();
	
	if (_blendStateAlpha) 
		_blendStateAlpha->Release();
	
	if (_blendStateOpaque) 
		_blendStateOpaque->Release();
	
	if (_depthStencilDisabled) 
		_depthStencilDisabled->Release();

	if (_blendStateAlpha)
		_blendStateAlpha->Release();

	if (_rasterizerWireframe)
		_rasterizerWireframe->Release();
	
}


void DXManager::ClearRenderTarget(float r, float g, float b, float a) 
{
	float color[4] = { r,g,b,a };

	// clear rendertarget and depth
	_devCon->ClearRenderTargetView(_renderTargetView, color);	
	_devCon->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1.0f);

}
void DXManager::PresentScene()
{
	//present depending on vsync
	if (_vsyncEnabled)			
		_swapChain->Present(1, 0);	
	else			
		_swapChain->Present(0, 0);	
}

void DXManager::SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencil, bool setDefault)
{
	setDefault ? _devCon->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView) : _devCon->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
}

void DXManager::SetViewport(D3D11_VIEWPORT* viewport, bool setDefault) 
{
	setDefault ? _devCon->RSSetViewports(1, &_viewport) : _devCon->RSSetViewports(1, viewport);
}

void DXManager::SetFullscreen(bool fullscreen, bool toggle)
{
	toggle ? _swapChain->SetFullscreenState(!_fullscreen, NULL) : _swapChain->SetFullscreenState(fullscreen, NULL);
	toggle ? _fullscreen = !_fullscreen : _fullscreen = fullscreen;
}

void DXManager::SetBlendState(BLEND_STATE blending)
{

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	switch (blending) 
	{
		case BLEND_STATE::BLEND_OPAQUE:
			_devCon->OMSetBlendState(_blendStateOpaque, blendFactor, 0xfffffff);
			break;
		case BLEND_STATE::BLEND_ALPHA:
			_devCon->OMSetBlendState(_blendStateAlpha, blendFactor, 0xfffffff);
			break;
		case BLEND_STATE::BLEND_ADDITIVE:
			_devCon->OMSetBlendState(_blendStateAddetive, blendFactor, 0xfffffff);
			break;	
		case BLEND_STATE::BLEND_SUBTRACTIVE:
			_devCon->OMSetBlendState(_blendStateSubtractive, blendFactor, 0xfffffff);
			break;
	}	 				 			
}

void DXManager::SetRasterizerState(RASTERIZER_STATE state)
{
	switch (state) 
	{
		case RASTERIZER_STATE::BACKCULL:
			_devCon->RSSetState(_rasterizerBackCull);
			break;
		case RASTERIZER_STATE::FRONTCULL:
			_devCon->RSSetState(_rasterizerFrontCull);
			break;
		case RASTERIZER_STATE::NOCULL:
			_devCon->RSSetState(_rasterizerNoCull);
			break;
		case RASTERIZER_STATE::WIREFRAME:
			_devCon->RSSetState(_rasterizerWireframe);
			break;	
	}					
}


void DXManager::SetZBuffer(DEPTH_STATE state)
{
	switch (state)
	{
	case ENABLED:
		_devCon->OMSetDepthStencilState(_depthStencilEnabled, 5);
		break;
	case DISABLED:
		_devCon->OMSetDepthStencilState(_depthStencilDisabled, 1);
		break;
	case READ_ONLY:
		_devCon->OMSetDepthStencilState(_depthStencilReadOnly, 1);
		break;
	case MASKED_SKYBOX:
		_devCon->OMSetDepthStencilState(_depthStencilMaskedSkybox, 5);
	}
}

void DXManager::Initialize(HWND hwnd,int screenWidth, int screenHeight, bool vsync, bool fullscreen)
{
	
	
	ID3D11Texture2D* backBufferPtr = 0;
	HRESULT result;

	//store vsync setting
	_vsyncEnabled = vsync;

	// get properties of monitors,gpu etc
	GetHardwareProperties(screenWidth, screenHeight);
		
	// create the swapchain
	if (!CreateSwapchain(hwnd, fullscreen, screenWidth, screenHeight)) 	
		printf("Failed to initialize swapchain");
		
	// get pointer to backbuffer	
	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr); 
	if (FAILED(result)) 	
		printf("Failed to get the backbuffer from swapchian");

	// create the main rendertarget and set it active
	result = _device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView);
	if (FAILED(result))	
		printf("Failed to create the rendertarget view");

	// delete texture after creation of rendertarget
	backBufferPtr->Release();
	
	// create the depth stencil buffer
	if (!CreateDepthStencil(screenWidth, screenHeight))	
		printf("Failed to initialize depthbuffer");

	// create the depthstencil states
	if (!CreateDepthstencilStates())	
		printf("Failed to initialize depthstencil state");
	
	//can set custum features how to render polygons
	if (!CreateRasterizerStates()) 	
		printf("Failed to initialize rasterizerstate");

	if (!CreateBlendStates()) 			
		printf("Failed to initialize alphablending");

	// create samplerstates for textures
	CreateSamplerStates();

	// setup viewport
	CreateViewport(screenWidth, screenHeight);
	
	_devCon->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	_devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
}

bool DXManager::CreateSwapchain(HWND hwnd, bool fullscreen, int screenWidth, int screenHeight) 
{
	// is inverted becuase swapchain takes a bool for windowed 
	_fullscreen = !fullscreen;

	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;

	//initialize swap chain description before we create the actual swap chain
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC)); 
	swapChainDesc.BufferCount = 1; // one back buffer
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // set the back buffer to 32-bit

	// set the refresh rate to match monitor if vSync is enabled, 
	if (_vsyncEnabled) 
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = _numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = _denominator;
	}
	else 
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	// set the usage of the backbuffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;		
	swapChainDesc.Windowed = _fullscreen;	
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow to switch to fullscreen using allt + enter
	featureLevel = D3D_FEATURE_LEVEL_11_1;
	

	// create the swapchain using the description, and devices
	result = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&_swapChain,
		&_device,
		NULL, &_devCon);

	if (FAILED(result)) 
		return false;
	
	return true;
}

bool DXManager::CreateDepthStencil(int screenWidth, int screenHeight) 
{
	HRESULT result;
	
	// set and create texture for depthbuffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ID3D11Texture2D* depthBuffer;
	ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; 															
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &depthBuffer);
	if (FAILED(result)) 
		return false;	

	// create the depthstencilview 
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = _device->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &_depthStencilView);
	if (FAILED(result))
		return false;

	depthBuffer->Release();

	return true;
}

bool DXManager::CreateDepthstencilStates()
{
	HRESULT result;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true; // enables depth
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; 
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 255;
	depthStencilDesc.StencilWriteMask = 255;

	// Stencil operations
	// replace the stencil value with the reference value on the rendered pixel no matter if the operation fails(only care that SOMETHING is rendered on the pixel)
	// the skybox will later mask the pixels that has the reference value set in the stencil
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE; 
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	
	// depth enabled
	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilEnabled);
	if (FAILED(result)) 
		return false;

	// will only read the depthbuffer, the depthstencil is ignored here aswell
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.StencilEnable = false;
	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilReadOnly);
	if (FAILED(result))
		return false;
			
	// will not read or write the deptbuffer and stencilbuffer
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = false;
	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilDisabled);
	if (FAILED(result)) 
		return false;

	// masked depthstencil with reference set to the same as the regular depthEnabled depthstencil
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.StencilEnable = true;
	
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL; 
	
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilMaskedSkybox);
	if (FAILED(result))
		return false;

	_devCon->OMSetDepthStencilState(_depthStencilEnabled, 1);
		
	return true;
}
bool DXManager::CreateRasterizerStates()
{
	HRESULT result;
	D3D11_RASTERIZER_DESC rasterDesc;
	
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// with backfaceculling
	result = _device->CreateRasterizerState(&rasterDesc, &_rasterizerBackCull);
	if (FAILED(result))	
		return false;
	
	// with frontfaceculling
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	result = _device->CreateRasterizerState(&rasterDesc, &_rasterizerFrontCull);
	if (FAILED(result))	
		return false;	

	// with no cull
	rasterDesc.CullMode = D3D11_CULL_NONE;
	result = _device->CreateRasterizerState(&rasterDesc, &_rasterizerNoCull);
	if (FAILED(result))
		return false;

	//wireframe
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	result = _device->CreateRasterizerState(&rasterDesc, &_rasterizerWireframe);
	if (FAILED(result))
		return false;

	// set the rasterizer state
	_devCon->RSSetState(_rasterizerBackCull);

	return true;
}

void DXManager::CreateViewport(int screenWidth, int screenHeight)
{		
	_viewport.Width = (float)screenWidth;
	_viewport.Height = (float)screenHeight;
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	_devCon->RSSetViewports(1, &_viewport);
}

bool DXManager::CreateBlendStates()
{
	HRESULT result;
	D3D11_BLEND_DESC blendStateDesc;

	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));

	// alphablending
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;	
	result = _device->CreateBlendState(&blendStateDesc, &_blendStateAlpha);
	if (FAILED(result))	
		return false;
	
	// solidblending
	blendStateDesc.RenderTarget[0].BlendEnable = false;		
	result = _device->CreateBlendState(&blendStateDesc, &_blendStateOpaque);
	if (FAILED(result))
		return false;
	
	//additiveblending
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	result = _device->CreateBlendState(&blendStateDesc, &_blendStateAddetive);
	if (FAILED(result))
		return false;

	// subtractiveblending
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE; 
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT; 
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	result = _device->CreateBlendState(&blendStateDesc, &_blendStateSubtractive);
	if (FAILED(result))	
		return false;
	
	return true;
}

void DXManager::CreateSamplerStates() 
{
	HRESULT result;
	D3D11_SAMPLER_DESC samplerDesc;

	// create samplerstate wrap
	
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; 
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = _device->CreateSamplerState(&samplerDesc, &_sampleStateWrap);
	if (FAILED(result))
		printf("failed to create samplestate\n");

	// samplerstate clamp
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = _device->CreateSamplerState(&samplerDesc, &_sampleStateClamp);
	if (FAILED(result))
		printf("failed to create samplestate\n");

	ID3D11SamplerState* samplers[2]{ _sampleStateWrap, _sampleStateClamp };

	_devCon->PSSetSamplers(0, 2, samplers);
}


void DXManager::GetHardwareProperties(int screenWidth, int screenHeight)
{	
	
	HRESULT result; 
	IDXGIFactory* factory; // used to create grapichal interfaces
	IDXGIAdapter* adapter; // use the adapter to get acces to the outputs(monitors), videomemory etc
	
	unsigned int numModes;
	size_t stringLenght;
	DXGI_MODE_DESC* displayModeList; // store all modes in here
	DXGI_ADAPTER_DESC adapterDesc; // store the description of our videocard here
	int error;

	//Create a DirectX graphic interface factory, used to create other graphical interfaces
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))	
		printf("Failed to create DXGIFactory");	

	// use the factory to create a adapter for the primary graphics interface(video card), we can then use the adapter to get info about our monitor/video memory etc
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))	
		printf("Failed to create adapter");
	

	// enumerate the primary adapter output(monitor) and store it in our adapterOutput
	result = adapter->EnumOutputs(0, &_adapterOutput);
	if (FAILED(result))	
		printf("Failed to enumerate the primer adapter output");
	

	//get the number of modes that fit the DXGI_FORMATR8G8B8A8_UNORM display format for the adapter output
	result = _adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))	
		printf("Failed to get display modes");

	// create a list to hold all the possible modes for this monitor/videocard combination
	displayModeList = new DXGI_MODE_DESC[numModes];

	//get all displaymodes
	result = _adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))	
		printf("Failed to create output mode list");
		
	// save the monitors refreshrate to be used if Vsync is enabled	
	for (int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth && displayModeList[i].Height == (unsigned int)screenHeight)
		{
			_numerator = displayModeList[i].RefreshRate.Numerator;
			_denominator = displayModeList[i].RefreshRate.Denominator;
			_refreshrate = (int)round((float)_numerator / _denominator);			
		}
	}
	if (_numerator == 0 && _denominator == 0)
		printf("your monitor do not support the current aspect ratio\n");

	//Get the adapter description and store in adapterDesc, contains info of our, monitor, videocard etc
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))	
		printf("Failed to get adapter discription");
	
	//store the videocard memory in mbs
	_videoCardMemory = (unsigned int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		
	// convert the name of videocard to character array
	error = wcstombs_s(&stringLenght, _videoCardDescriptiion, 128, adapterDesc.Description, 128);
	if (error != 0)	
		printf("Failed to convert the videocard info");	

	// cleanup
	delete[] displayModeList;
	adapter->Release();
	factory->Release();
}

