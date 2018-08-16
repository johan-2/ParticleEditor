#include "Framework.h"
#include "SystemDefs.h"
#include "DXManager.h"
#include "CameraManager.h"
#include "ShaderManager.h"
#include "LightManager.h"
#include "Renderer.h"
#include "World.h"
#include "Input.h"
#include "Time.h"
#include "Mesh.h"
#include "Entity.h"
#include "FreeMoveComponent.h"
#include "TransformationComponent.h"
#include "UVScrollComponent.h"
#include "CameraComponent.h"
#include "QuadComponent.h"
#include "ModelComponent.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "GuiManager.h"
#include "DebugStats.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "TexturePool.h"
#include "Skybox.h"
#include <string>
#include <algorithm>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM Lparam);

Framework::Framework()
{
	_clearColor[0] = 1.0f;
	_clearColor[1] = 0.0f;
	_clearColor[2] = 0.0f;
	_clearColor[3] = 1.0f;

	// create window
	CreateWindowDx11("Engine", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// init all directX stuff
	DXManager::GetInstance().Initialize(_window, SCREEN_WIDTH, SCREEN_HEIGHT, V_SYNC, FULLSCREEN);

	GuiManager::GetInstance().Initialize(_window);
	// load and create our shader objects	
	ShaderManager::GetInstance().Initialize();

	// initialize directinput
	Input::GetInstance().InitializeInputDevices(_hInstance, _window);

	// start timers
	Time::GetInstance();

	// start and run
	Start();
	Run();
	
}

Framework::~Framework()
{
	DXManager::GetInstance().Shutdown();
	ShaderManager::GetInstance().Shutdown();
	
	UnregisterClass((LPCSTR)_applicationName.c_str(), _hInstance);	
}

void Framework::Start()
{

	// setup renderer to handle depthrendering
	Renderer::GetInstance().CreateDepthMap();

	// create game camera
	Entity* cameraGame = new Entity();
	cameraGame->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 8, -15), XMFLOAT3(15, 0, 0));
	cameraGame->AddComponent<CameraComponent>()->Init3D(70); 
	cameraGame->AddComponent<FreeMoveComponent>()->init();
	CameraManager::GetInstance().SetCurrentCameraGame(cameraGame->GetComponent<CameraComponent>());

	// create UIcamera
	Entity* cameraUI = new Entity();
	cameraUI->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, -1));
	cameraUI->AddComponent<CameraComponent>()->Init2D(XMFLOAT2(SCREEN_WIDTH, SCREEN_HEIGHT), XMFLOAT2(0.01f, 1.0f));
	CameraManager::GetInstance().SetCurrentCameraUI(cameraUI->GetComponent<CameraComponent>());

	//set ambient light color	
	LightManager::GetInstance().SetAmbientColor(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));

	// get transform of the camera that renders the depthmap
	TransformComponent* dt = CameraManager::GetInstance().GetCurrentCameraDepthMap()->GetComponent<TransformComponent>();

	// create directional light and give it the same position/ rotation as depthrender camera
	Entity* directionalLight = new Entity;
	directionalLight->AddComponent<TransformComponent>()->Init(dt->GetPositionVal(), dt->GetRotationVal());
	directionalLight->AddComponent<LightDirectionComponent>()->Init(XMFLOAT4(0.8f, 0.8f, 0.8f, 1), XMFLOAT4(1, 1, 1, 1), 100.0f);


	_grid = new Entity();
	_grid->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0));
	_grid->AddComponent<ModelComponent>()->InitGrid(100); 

	_particleEntity = new Entity();
	_transform = _particleEntity->AddComponent<TransformComponent>(); _transform->Init();
	_transformation = _particleEntity->AddComponent<TransformationComponent>(); _transformation->Init(XMFLOAT3(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, TransformComponent::Axis::RIGHT);
	_particleEntity->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, AMBIENT);
	_particleComponent = _particleEntity->AddComponent<ParticleEmitterComponent>(); _particleComponent->Init("Particles/multipleEmittersTest.Json");

	_numEmitters = _particleComponent->GetNumEmitters();

	for (int i = 0; i < _numEmitters; i++)
	{
		_particleSettings.emplace_back(_particleComponent->GetSettings(i));
		_blendEnum.emplace_back(_particleComponent->GetBlendState(i) - 1);
	}

	_currentEmitterIndex = 0;
	
}

void Framework::Update()
{
	World::GetInstance().Update();

	UpdateInfoWindow();
	UpdateParticleSettingsWindow();
	UpdateEditorSettingsWindow();

	if (Input::GetInstance().IskeyPressed(DIK_F2))
		ReloadEmitter();

}

void Framework::UpdateParticleSettingsWindow()
{
	bool m = true;

	// settings window	
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.21f, SCREEN_HEIGHT * 0.8f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.01f, SCREEN_HEIGHT * 0.01f), 0, ImVec2(0, 0));
	ImGui::Begin("Particle Settings", &m, ImGuiWindowFlags_HorizontalScrollbar);

	// texture
	if (ImGui::Button("FIND"))
	{
		char filename[MAX_PATH];
		ZeroMemory(&filename, sizeof(filename));
		char oldDir[MAX_PATH];
		ZeroMemory(&oldDir, sizeof(oldDir));

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = ".dds\0*.dds";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "Select dds texture";
		ofn.Flags = 0;

		GetCurrentDirectory(MAX_PATH, oldDir); // we have to save the current directory before we open the openfile directory so we can set it back efter we have selected a file
											   // it will permanantly change our base directory making searches from $SolutionDir/ not work anymore if we not
		if (GetOpenFileName(&ofn))
		{
			SetCurrentDirectory(oldDir); // set back to old directory

			std::string name(filename);
			const size_t lastSlash = name.find_last_of("\\");
			if (std::string::npos != lastSlash)
				name.erase(0, lastSlash + 1);

			_particleSettings[_currentEmitterIndex].texturePath = name;
		}
		ReloadEmitter();
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputText("01", (char*)_particleSettings[_currentEmitterIndex].texturePath.c_str(), 30);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Texture");
	ShowToolTip("The Location that the texture is loaded from (Textures/ is added before name by defualt)\n- Press find to open fileDirectory");
	ImGui::PopItemWidth();

	// num particles
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputInt("02", (int*)&_particleSettings[_currentEmitterIndex].numParticles);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Num Particles");
	ShowToolTip("Number of particles in emitter, spawned at once if BURST is true\n else is spawned out with delay of particleLifespan/NumParticles");
	ImGui::PopItemWidth();

	// spawnradius
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat("03", &_particleSettings[_currentEmitterIndex].spawnRadius, 0.1f, 0, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "SpawnRadius");
	ShowToolTip("Randomize offset from emitter position - Spawnradius / 2 and emitter position + Spawnradius / 2");
	ImGui::PopItemWidth();

	//spawnOffset
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("04", &_particleSettings[_currentEmitterIndex].spawnOffset.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Spawn Offset");
	ShowToolTip("The offset from emitter position that the particle will consider its origin");
	ImGui::PopItemWidth();

	//startsize
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("05", &_particleSettings[_currentEmitterIndex].minMaxSpeed.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Speed Min Max");
	ShowToolTip("will randomize speed from value 1 and value 2");
	ImGui::PopItemWidth();

	//startsize
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("06", &_particleSettings[_currentEmitterIndex].startSize.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Start Size");
	ShowToolTip("StartSize in units");
	ImGui::PopItemWidth();

	//startScaleMinMax
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("07", &_particleSettings[_currentEmitterIndex].startScaleMinMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "StartScale Min Max");
	ShowToolTip("Will randomize a start scale between value 1 and value 2");
	ImGui::PopItemWidth();

	//endScaleMinMax
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("08", &_particleSettings[_currentEmitterIndex].endScaleMinMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "EndScale Min Max");
	ShowToolTip("Will randomize a end scale between value 1 and value 2\n will linearly interpolate between startScaleMinMax and this during the lifetime of the particle");
	ImGui::PopItemWidth();

	//Move direction
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("09", &_particleSettings[_currentEmitterIndex].direction.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Move Direction");
	ShowToolTip("The direction the particle will be spawned in");
	ImGui::PopItemWidth();

	//spread
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("10", &_particleSettings[_currentEmitterIndex].velocitySpread.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Directional Spread");
	ShowToolTip("Will be calculating a random start direction between:\n- Move Direction - Spread / 2 and Move Direction + Spread / 2\n So if you have a direction set to 1 and spread to 4 it will result of a value between -1 and 3\n Making it a little moore possible that it will go towards the set direction");
	ImGui::PopItemWidth();

	//gravity
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("11", &_particleSettings[_currentEmitterIndex].gravity.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Gravity");
	ShowToolTip("- Gravity will over time reduce or increase the amount of speed to Move Direction\n- Gravity is also added on to the velocity, so even if your speed gets decreased to 0 the particle will still move accoring to gravity\n- Ex, if you have positive x direction and positive x gravity the particle speed will only increase to the right\n- Ex, if you have positive x direction and negative x gravity the particle will start moving to the right but over time change to the left giving gravity hierarchy");
	ImGui::PopItemWidth();

	// Drag
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat("12", &_particleSettings[_currentEmitterIndex].drag, 0.01f, 0, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Drag");
	ShowToolTip("- Drag will slow your particle down with time\n- High drag will make the particle completely idle with time\n- Effect both speed and gravity");
	ImGui::PopItemWidth();

	// emitter lifetime
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat("13", &_particleSettings[_currentEmitterIndex].emitterLifetime, 0.1f, 0, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Emitter Lifetime");
	ShowToolTip("Time before the emitter is removed (0 is magic number for infinite) NOTE: Will remove entity in editor so should only be set before saving your particle if used");
	ImGui::PopItemWidth();

	// particle lifetime
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat("14", &_particleSettings[_currentEmitterIndex].particleLifetime, 0.1f, 0, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Particle Lifetime");
	ShowToolTip("Time before particle will be reset and respawned");
	ImGui::PopItemWidth();

	// start alpha
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::SliderFloat("15", &_particleSettings[_currentEmitterIndex].startAlpha, 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Start Alpha");
	ShowToolTip("Alpha channel start, interpolated during lifetime (works the same no matter blendstate)");
	ImGui::PopItemWidth();

	// end alpha
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::SliderFloat("16", &_particleSettings[_currentEmitterIndex].endAlpha, 0.0f, 1.0f, "%.2f");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "End Alpha");
	ShowToolTip("Alpha channel End, interpolated during lifetime (works the same no matter blendstate)");
	ImGui::PopItemWidth();

	//StartColor min
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("17", &_particleSettings[_currentEmitterIndex].startColorMultiplierRGBMin.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "StartColor Multiplier Min");
	ShowToolTip("will be randomized between this and StartColor Multiplier Max\n-Will take color value of texture and multiply it by x times\n- 0.0 to 0.99 will remove color from channel\n- 1.0 to infinite will intensify color channel\n- Ex, red color channel at 0.5 with multiplier of 0.5 will result of a red color channel at 0.25 making it 50%% weaker\n- Ex, channel of 1.0f with multiplier of 0.95 will give result of a 5%% decrease\n- Ex, channel of 0.2 with multipler of 3 will result in 0.6 ");
	ImGui::PopItemWidth();

	//startcolor max
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("18", &_particleSettings[_currentEmitterIndex].startColorMultiplierRGBMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "StartColor multiplier Max");
	ShowToolTip("will be randomized between this and StartColor Multiplier Min\n-Will take color value of texture and multiply it by x times\n- 0.0 to 0.99 will remove color from channel\n- 1.0 to infinite will intensify color channel\n- Ex, red color channel at 0.5 with multiplier of 0.5 will result of a red color channel at 0.25 making it 50%% weaker\n- Ex, channel of 1.0f with multiplier of 0.95 will give result of a 5%% decrease\n- Ex, channel of 0.2 with multipler of 3 will result in 0.6 ");
	ImGui::PopItemWidth();

	//endcolor min
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("19", &_particleSettings[_currentEmitterIndex].endColorMultiplierRGBMin.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "EndColor Multiplier Min");
	ShowToolTip("will be randomized between this and EndColor Multiplier Max\n-Will take color value of texture and multiply it by x times\n- 0.0 to 0.99 will remove color from channel\n- 1.0 to infinite will intensify color channel\n- Ex, red color channel at 0.5 with multiplier of 0.5 will result of a red color channel at 0.25 making it 50%% weaker\n- Ex, channel of 1.0f with multiplier of 0.95 will give result of a 5%% decrease\n- Ex, channel of 0.2 with multipler of 3 will result in 0.6 ");
	ImGui::PopItemWidth();

	//endcolor max
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("20", &_particleSettings[_currentEmitterIndex].endColorMultiplierRGBMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "EndColor Multiplier Max");
	ShowToolTip("will be randomized between this and EndColor Multiplier Min\n-Will take color value of texture and multiply it by x times\n- 0.0 to 0.99 will remove color from channel\n- 1.0 to infinite will intensify color channel\n- Ex, red color channel at 0.5 with multiplier of 0.5 will result of a red color channel at 0.25 making it 50%% weaker\n- Ex, channel of 1.0f with multiplier of 0.95 will give result of a 5%% decrease\n- Ex, channel of 0.2 with multipler of 3 will result in 0.6 ");
	ImGui::PopItemWidth();

	//endcolor max
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("30", &_particleSettings[_currentEmitterIndex].inheritVelocityScale.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Enherit Velocity scale");
	ShowToolTip("How much velocity the particle will inherit from movement of emitter transform");
	ImGui::PopItemWidth();

	//rotation per sec minmax
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("21", &_particleSettings[_currentEmitterIndex].rotationPerSecMinMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Rotation Per Second");
	ShowToolTip("Will rotate the particle x amount per second on the local z axis (will not affect billboarding)\n- Rotation by velocity need to be set to false to use this feature");
	ImGui::PopItemWidth();

	//uv scroll x minmax
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("22", &_particleSettings[_currentEmitterIndex].uvScrollXMinMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Uv Scroll X min max");
	ShowToolTip("Will scroll the U coordinate of texture");
	ImGui::PopItemWidth();

	//uv scroll y minmax
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat2("23", &_particleSettings[_currentEmitterIndex].uvScrollYMinMax.x, 2);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Uv Scroll Y min max");
	ShowToolTip("Will scroll the V coordinate of texture");
	ImGui::PopItemWidth();

	// burst
	ImGui::Checkbox("24", &_particleSettings[_currentEmitterIndex].burst);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Burst");
	ShowToolTip("If all particles should spawn at once or not");

	// follow emitter
	ImGui::Checkbox("25", &_particleSettings[_currentEmitterIndex].followEmitter);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Follow emitter");
	ShowToolTip("If the particles position should be relative to the position of the emitter");

	// local space
	ImGui::Checkbox("26", &_particleSettings[_currentEmitterIndex].LocalSpace);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Local Space");
	ShowToolTip("If the move Direction should be based on the emitters local orientation or not\n- Is only Set when spawned");

	// rotation by velocity
	ImGui::Checkbox("27", &_particleSettings[_currentEmitterIndex].rotationByVelocity);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Rotation by velocity");
	ShowToolTip("Makes the up axis always point towards the direction of velocity");

	// blendstate
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::Combo("28", &_blendEnum[_currentEmitterIndex], "ALPHA\0ADDITIVE\0SUBTRACTIVE");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Blend State");
	ShowToolTip("Particle blending that will be used");
	ImGui::PopItemWidth();

	// texture image preview
	ImGui::Image((void*)_particleComponent->GetTexture(_currentEmitterIndex), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

	// add a new emitter
	if (ImGui::Button("Add New Emitter"))
	{
		if (_numEmitters < 8)
		{
			ParticleSettings newSettings = _particleSettings[_currentEmitterIndex];
			_particleSettings.push_back(newSettings);
			_blendEnum.push_back(_blendEnum[_currentEmitterIndex]);
			_numEmitters++;
			_currentEmitterIndex = _numEmitters - 1; // set the current emitter to the newly created
			ReloadEmitter();
		}
	}

	// remove current selected emitter
	if (ImGui::Button("Remove Selected Emitter"))
	{
		if (_numEmitters >1)
		{
			_particleSettings.erase(_particleSettings.begin() + _currentEmitterIndex);
			_blendEnum.erase(_blendEnum.begin() + _currentEmitterIndex);

			_numEmitters--;
			_currentEmitterIndex = 0;
			ReloadEmitter();
		}
	}

	// select witch emitter to edit
	ImGui::Combo("Emitter to edit", (int*)&_currentEmitterIndex, GetNumEmittersAsString());

	// apply button
	ImGui::Spacing();
	if (ImGui::Button("APPLY SETTINGS"))
		ReloadEmitter();

	ImGui::End();
}

void Framework::UpdateEditorSettingsWindow()
{
	bool m = true;

	// editor settings window
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.20f, SCREEN_HEIGHT * 0.5f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.99f, SCREEN_HEIGHT * 0.01f), 0, ImVec2(1, 0));
	ImGui::Begin("Editor Settings", &m, ImGuiWindowFlags_HorizontalScrollbar);

	// skybox
	ImGui::Checkbox("Render Skybox", ShaderManager::GetInstance().GetrenderSkybox());

	// load skybox .dds cubemap
	if (ImGui::Button("Load Skybox"))
	{
		char filename[MAX_PATH];
		ZeroMemory(&filename, sizeof(filename));
		char oldDir[MAX_PATH];
		ZeroMemory(&oldDir, sizeof(oldDir));

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = ".dds\0*.dds";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "select dds cubemap";
		ofn.Flags = 0;

		GetCurrentDirectory(MAX_PATH, oldDir); // we have to save the current directory before we open the openfile directory so we can set it back efter we have selected a file
											   // it will permanantly change our base directory making searches from $SolutionDir/ not work anymore if we not
		if (GetOpenFileName(&ofn))
		{
			SetCurrentDirectory(oldDir); // set back to old directory
			Renderer::GetInstance().GetSkybox()->SetSkyBox(filename);
		}
		ReloadEmitter();
	}

	// clear color
	if (ImGui::ColorButton("color", ImVec4(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3])))
		ImGui::OpenPopup("picker");

	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1.0), "Clear Color");
	ShowToolTip("Skybox need to be off to use clear color");
	if (ImGui::BeginPopup("picker"))
	{
		ImGui::ColorPicker4("clearPicker", _clearColor);
		ImGui::EndPopup();
	}

	// emitter rendermode
	ImGui::Checkbox("Emitter as wireframe", &_particleEntity->GetComponent<ModelComponent>()->GetMesh()->_wireFrame);

	// show grid
	ImGui::Checkbox("ShowGrid", &_grid->GetComponent<ModelComponent>()->GetMesh()->_render);

	// emitter rotation
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputFloat3("Emitter Rotation", (float*)_transformation->GetRotationPtr(), 2);
	ImGui::PopItemWidth();

	// emitter translation
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	static int moveEnum = _transformation->_state;
	ImGui::Combo("Emitter Translation", &moveEnum, "IDLE\0CIRCLE\0BACKFORTH");
	_transformation->_state = (MoveState)moveEnum;
	ImGui::PopItemWidth();

	// reset emitter transform
	if (ImGui::Button("Reset Transform"))
	{
		_transform->GetPositionRef() = XMFLOAT3(0, 0, 0);
		_transform->GetRotationRef() = XMFLOAT3(0, 0, 0);
	}

	ImGui::Spacing();
	if (ImGui::Button("Load Particle from file"))
	{
		char filename[MAX_PATH];
		ZeroMemory(&filename, sizeof(filename));
		char oldDir[MAX_PATH];
		ZeroMemory(&oldDir, sizeof(oldDir));

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = ".json\0*.json";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "Select particle JsonFile";
		ofn.Flags = 0;

		GetCurrentDirectory(MAX_PATH, oldDir); // we have to save the current directory before we open the openfile directory so we can set it back efter we have selected a file
											   // it will permanantly change our base directory making searches from $SolutionDir/ not work anymore if we not
		if (GetOpenFileName(&ofn))
		{
			SetCurrentDirectory(oldDir); // set back to old directory
			_particleEntity->RemoveComponent(_particleComponent);
			_particleComponent = _particleEntity->AddComponent<ParticleEmitterComponent>(); _particleComponent->Init(filename);

			_particleSettings.clear();
			_blendEnum.clear();

			_numEmitters = _particleComponent->GetNumEmitters();

			for (int i = 0; i < _numEmitters; i++)
			{
				_particleSettings.emplace_back(_particleComponent->GetSettings(i));
				_blendEnum.emplace_back(_particleComponent->GetBlendState(i) - 1);
			}
			_currentEmitterIndex = 0;
		}
		ReloadEmitter();
	}

	if (ImGui::Button("Save To file"))
	{
		char filename[MAX_PATH];
		ZeroMemory(&filename, sizeof(filename));
		char oldDir[MAX_PATH];
		ZeroMemory(&oldDir, sizeof(oldDir));

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;
		ofn.lpstrFilter = ".json\0*.json";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = "save particle as .json";
		ofn.Flags = 0;
		ofn.lpstrDefExt = ".json";

		GetCurrentDirectory(MAX_PATH, oldDir); // we have to save the current directory before we open the openfile directory so we can set it back efter we have selected a file
											   // it will permanantly change our base directory making searches from $SolutionDir/ not work anymore if we not		
		if (GetSaveFileName(&ofn))
		{
			SetCurrentDirectory(oldDir); // set back to old directory
			SaveParticle(filename);
		}
		ReloadEmitter();
	}

	ImGui::End();
}

void Framework::UpdateInfoWindow()
{
	bool m = true;

	// info window with short commands	
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.1f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.01f, SCREEN_HEIGHT * 0.95f), 0, ImVec2(0, 1));

	ImGui::Begin("short commands", &m, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

	ImGuiIO& io = ImGui::GetIO();

	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "FPS : %.2f", io.Framerate);

	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "PRESS");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "F1");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "TO TOGGLE FREE MOVE CAMERA");

	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "PRESS");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 1), "F2");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "TO APPLY CHANGES");

	ImGui::End();
}

void Framework::ShowToolTip(const char* tip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::SetTooltip(tip);
		ImGui::EndTooltip();
	}
}

char* Framework::GetNumEmittersAsString()
{
	// this is awful but I cant conjugate a string with "\0" and imgui dont allow "\\0" in a string for seperating objects in combobox
	switch (_numEmitters)
	{
	case 0:
		return "No active emitters";
	case 1:
		return "Emitter1";
	case 2:
		return "Emitter1\0Emitter2";
	case 3:
		return "Emitter1\0Emitter2\0Emitter3";
	case 4:
		return "Emitter1\0Emitter2\0Emitter3\0Emitter4";
	case 5:
		return "Emitter1\0Emitter2\0Emitter3\0Emitter4\0Emitter5";
	case 6:
		return "Emitter1\0Emitter2\0Emitter3\0Emitter4\0Emitter5\0Emitter6";
	case 7:
		return "Emitter1\0Emitter2\0Emitter3\0Emitter4\0Emitter5\0Emitter6\0Emitter7";
	case 8:
		return "Emitter1\0Emitter2\0Emitter3\0Emitter4\0Emitter5\0Emitter6\0Emitter7\0Emitter8";

	default:
		return "";
		break;
	}
}

void Framework::ReloadEmitter()
{
	for (int i = 0; i< _numEmitters; i++)
	{
		int b = _blendEnum[i] + 1; // re apply 1 to get the correct enum index according to the blendstate enums in engine
		_particleSettings[i].BLEND = (BLEND_STATE)b;
	}

	_particleEntity->RemoveComponent(_particleComponent);
	_particleComponent = _particleEntity->AddComponent<ParticleEmitterComponent>(); _particleComponent->Init(_particleSettings, _numEmitters);
}

void Framework::SaveParticle(char* destination)
{
	// CODE FOR OUTPUTING ALL VALUES
	rapidjson::StringBuffer sb;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);

	writer.StartObject();

	writer.Key("numEmitters");
	writer.Int(_numEmitters);

	std::string key = "";

	for (int i = 0; i < _numEmitters; i++)
	{
		std::string index = std::to_string(i);

		key = "numParticles";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Int(_particleSettings[i].numParticles);

		key = "texture";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.String(_particleSettings[i].texturePath.c_str());

		key = "startSize";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].startSize.x);
		writer.Double(_particleSettings[i].startSize.y);
		writer.EndArray();

		key = "direction";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].direction.x);
		writer.Double(_particleSettings[i].direction.y);
		writer.Double(_particleSettings[i].direction.z);
		writer.EndArray();

		key = "minMaxSpeed";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].minMaxSpeed.x);
		writer.Double(_particleSettings[i].minMaxSpeed.y);
		writer.EndArray();

		key = "gravity";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].gravity.x);
		writer.Double(_particleSettings[i].gravity.y);
		writer.Double(_particleSettings[i].gravity.z);
		writer.EndArray();

		key = "drag";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].drag);

		key = "velocitySpread";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].velocitySpread.x);
		writer.Double(_particleSettings[i].velocitySpread.y);
		writer.Double(_particleSettings[i].velocitySpread.z);
		writer.EndArray();

		key = "emitterLifetime";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].emitterLifetime);

		key = "particleLifetime";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].particleLifetime);

		key = "spawnRadius";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].spawnRadius);

		key = "burst";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Bool(_particleSettings[i].burst);

		key = "followEmitter";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Bool(_particleSettings[i].followEmitter);

		key = "startColorMultiplierRGBMin";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].startColorMultiplierRGBMin.x);
		writer.Double(_particleSettings[i].startColorMultiplierRGBMin.y);
		writer.Double(_particleSettings[i].startColorMultiplierRGBMin.z);
		writer.EndArray();

		key = "startColorMultiplierRGBMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].startColorMultiplierRGBMax.x);
		writer.Double(_particleSettings[i].startColorMultiplierRGBMax.y);
		writer.Double(_particleSettings[i].startColorMultiplierRGBMax.z);
		writer.EndArray();

		key = "endColorMultiplierRGBMin";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].endColorMultiplierRGBMin.x);
		writer.Double(_particleSettings[i].endColorMultiplierRGBMin.y);
		writer.Double(_particleSettings[i].endColorMultiplierRGBMin.z);
		writer.EndArray();

		key = "endColorMultiplierRGBMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].endColorMultiplierRGBMax.x);
		writer.Double(_particleSettings[i].endColorMultiplierRGBMax.y);
		writer.Double(_particleSettings[i].endColorMultiplierRGBMax.z);
		writer.EndArray();

		key = "startAlpha";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].startAlpha);

		key = "endAlpha";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Double(_particleSettings[i].endAlpha);

		key = "BLEND";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Int((int)_particleSettings[i].BLEND);

		key = "spawnOffset";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].spawnOffset.x);
		writer.Double(_particleSettings[i].spawnOffset.y);
		writer.Double(_particleSettings[i].spawnOffset.z);
		writer.EndArray();

		key = "localSpace";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Bool(_particleSettings[i].LocalSpace);

		key = "startScaleMinMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].startScaleMinMax.x);
		writer.Double(_particleSettings[i].startScaleMinMax.y);
		writer.EndArray();

		key = "endScaleMinMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].endScaleMinMax.x);
		writer.Double(_particleSettings[i].endScaleMinMax.y);
		writer.EndArray();

		key = "rotationByVelocity";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.Bool(_particleSettings[i].rotationByVelocity);

		key = "rotationPerSecMinMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].rotationPerSecMinMax.x);
		writer.Double(_particleSettings[i].rotationPerSecMinMax.y);
		writer.EndArray();

		key = "uvScrollXMinMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].uvScrollXMinMax.x);
		writer.Double(_particleSettings[i].uvScrollXMinMax.y);
		writer.EndArray();

		key = "uvScrollYMinMax";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].uvScrollYMinMax.x);
		writer.Double(_particleSettings[i].uvScrollYMinMax.y);
		writer.EndArray();

		key = "inheritVelocityScale";
		key.append(index.c_str());
		writer.Key(key.c_str());
		writer.StartArray();
		writer.Double(_particleSettings[i].inheritVelocityScale.x);
		writer.Double(_particleSettings[i].inheritVelocityScale.y);
		writer.Double(_particleSettings[i].inheritVelocityScale.z);
		writer.EndArray();
	}

	writer.EndObject();

	std::ofstream of(destination);
	of << sb.GetString();

	of.close();
}

void Framework::Render()
{
	Renderer::GetInstance().Render();
	ShaderManager::GetInstance().RenderGUI(ImGui::GetDrawData());
}

void Framework::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// update imgui
		GuiManager::GetInstance().Update();

		// update timers
		Time::GetInstance().Update();

		//uppdate input
		Input::GetInstance().Update();
		
		//destroy window if escape is pressed, will send quit message with windowproc to end loop
		if (Input::GetInstance().IskeyPressed(DIK_ESCAPE))
		{
			DXManager::GetInstance().SetFullscreen(false);
			DestroyWindow(_window);
		}

		// update everything
		Update();
		
		DXManager& DXM = DXManager::GetInstance();

		// clear rendertarget from last frame
		DXManager::GetInstance().ClearRenderTarget(_clearColor[0], _clearColor[1], _clearColor[2], _clearColor[3]);

		// setup ImGui buffers before rendering
		ImGui::Render();

		// render everything
		Render();

		// swap buffers
		DXM.PresentScene();
	}
}

void Framework::CreateWindowDx11(char* title, int x, int y, int width, int height)
{
	_applicationName = title;
	HWND hwnd;
	WNDCLASSEX wc;
	
	_hInstance = GetModuleHandle(NULL);

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// set up window class with default settings
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc; // tell witch function that will be used when receving evnts from windows
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;
	wc.cbSize = sizeof(WNDCLASSEX);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "failed to register windowClass", "ERROR", MB_OK);
		return;
	}
	// create the window 	
	int nStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		title,
		title,
		nStyle,
		x,
		y,
		width,
		height,
		NULL,
		NULL,
		_hInstance,
		NULL);

	// if window is null something went wrong and we return false
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Create window failed", "ERROR", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// set focus on window
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	_window = hwnd;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM Lparam)
{
	// just handle closing, destroying and quit messages with wndproc, all other input is done with directinput
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		SetFocus(GetConsoleWindow());
		break;
	case WM_SETCURSOR:
		GuiManager::GetInstance().UpdateMouseCursor();
		break;
	case WM_WINDOWPOSCHANGING:
		Time::GetInstance().OnWindowChange();
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, Lparam);
	}

	return 0;
}