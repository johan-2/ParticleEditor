#include "Systems.h";

DXManager*     Systems::dxManager     = nullptr;
GuiManager*    Systems::guiManager    = nullptr;
Input*         Systems::input         = nullptr;
Time*          Systems::time          = nullptr;
CameraManager* Systems::cameraManager = nullptr;
LightManager*  Systems::lightManager  = nullptr;
TexturePool*   Systems::texturePool   = nullptr;
Renderer*      Systems::renderer      = nullptr;
World*         Systems::world         = nullptr;

Systems::Systems(){}
Systems::~Systems(){}

void Systems::InitSystems(Window* window, float screenWidth, float screenHeight, bool fullscreen, bool vSync)
{
	// create DX Manager
	dxManager = new DXManager();
	dxManager->Initialize(window->GetHWND(), screenWidth, screenHeight, vSync, fullscreen);

	// create GUI Manager
	guiManager = new GuiManager();
	guiManager->Initialize(window->GetHWND());

	// Create Input Handler
	input = new Input();
	input->InitializeInputDevices(window->GetHINSTANCE(), window->GetHWND());

	// Create misc systems
	time          = new Time();
	cameraManager = new CameraManager();
	lightManager  = new LightManager();
	texturePool   = new TexturePool();
	world         = new World();

	// Create the renderer
	renderer = new Renderer();
	renderer->Initailize();
}