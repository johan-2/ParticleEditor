#include "ParticleEditor.h"
#include "Entity.h"
#include "IComponent.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "Mesh.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/prettywriter.h"
#include "TexturePool.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include "GuiManager.h"
#include "SystemDefs.h"

ParticleEditor::ParticleEditor()
{
}

ParticleEditor::~ParticleEditor()
{
}

void ParticleEditor::Setup()
{
	// create a grid
	_grid = new Entity();
	_grid->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0, 0));
	_grid->AddComponent<ModelComponent>()->InitGrid(100, 1, Color32(255, 0, 0, 1));

	// create particle system entity
	_particleEntity = new Entity();
	_particleEntity->AddComponent<TransformComponent>()->Init(XMFLOAT3(0, 0.5f, 0)); 
	_particleEntity->AddComponent<ModelComponent>()->InitPrimitive(PRIMITIVE_TYPE::CUBE, WIREFRAME_COLOR);
	_particleEntity->AddComponent<ParticleSystemComponent>()->Init("Particles/fire.json");

	// cache components
	_systemTransformComponent = _particleEntity->GetComponent<TransformComponent>();
	_systemParticleComponent  = _particleEntity->GetComponent<ParticleSystemComponent>();

	// get how many emitters exist in the start particle system
	_numEmitters = _systemParticleComponent->GetNumEmitters();

	// loop over all emitters and get a cache of the settings and the blend states
	for (int i = 0; i < _numEmitters; i++)
	{
		_particleSettings.emplace_back(_systemParticleComponent->GetSettings(i));

		// note: the first blend state in our enum is opaque which the particles dont use
		// remove 1 from the enum so we can use our enums 1-3 as index 0-3
		_blendEnum.emplace_back(_systemParticleComponent->GetBlendState(i) - 1);
	}

	// start editing the first defined emitter in the system
	_currentEmitterIndex = 0;
}

void ParticleEditor::Update()
{
	UpdateParticleSettingsWindow();
}

void ParticleEditor::UpdateParticleSettingsWindow()
{
	// set properties of this GUI window
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.21f, SCREEN_HEIGHT * 0.8f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.01f, SCREEN_HEIGHT * 0.01f), 0, ImVec2(0, 0));
	ImGui::Begin("Particle Settings", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

	// texture
	if (ImGui::Button("FIND"))
	{
		// create memory for the path to the texture file we select
		char filename[MAX_PATH];
		ZeroMemory(&filename, sizeof(filename));

		// create memory for the path to our solution directory
		char oldDir[MAX_PATH];
		ZeroMemory(&oldDir, sizeof(oldDir));

		// create settings for the open file directory window
		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner   = NULL;
		ofn.lpstrFilter = ".dds\0*.dds";
		ofn.lpstrFile   = filename;
		ofn.nMaxFile    = MAX_PATH;
		ofn.lpstrTitle  = "Select dds texture";
		ofn.Flags = 0;

		// we have to save the current directory before we open the openfile directory so we can set it back efter we have selected a file
		// it will permanantly change our base directory making searches from $SolutionDir/ not work anymore if we not
		GetCurrentDirectory(MAX_PATH, oldDir); 
											   
		if (GetOpenFileName(&ofn))
		{
			// set back to old directory
			SetCurrentDirectory(oldDir); 

			// put the file directory in a string
			std::string name(filename);

			// get the offset where our last backslash is located
			// we only want to save the name of the texture and later
			// use a relative path to find and load it
			const size_t lastSlash = name.find_last_of("\\");

			// erease filepath
			if (std::string::npos != lastSlash)
				name.erase(0, lastSlash + 1);

			// set the new texture in our settings of this emitter
			_particleSettings[_currentEmitterIndex].texturePath = name;
		}

		// reload the emitter so the texture is changed
		ReloadEmitter();
	}

	// texture input
	ImGui::SameLine();
	ImGui::PushItemWidth(SCREEN_WIDTH * 0.09f);
	ImGui::InputText("01", (char*)_particleSettings[_currentEmitterIndex].texturePath.c_str(), 30);
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.9, 0.9, 0.9, 1), "Texture");
	ShowToolTip("The Location that the texture is loaded from (Textures/ is added before name by defualt)\n- Press find to open fileDirectory to search for a texture");
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
	ImGui::Image((void*)_systemParticleComponent->GetTexture(_currentEmitterIndex), ImVec2(50, 50), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(1, 1, 1, 1));

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
		if (_numEmitters > 1)
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

void ParticleEditor::ShowToolTip(const char* tip)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::SetTooltip(tip);
		ImGui::EndTooltip();
	}
}

char* ParticleEditor::GetNumEmittersAsString()
{
	// this is not very nice but IM gui seperates items in a combobox by adding "\0"
	// between each item in a single string and I can't conjugate
	// a string by adding a single "\" into it and im gui does not
	// accept "\\0" as a valid input seperator so this is the
	// best temporary solution I could think of for now
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

void ParticleEditor::ReloadEmitter()
{
	for (int i = 0; i < _numEmitters; i++)
	{
		// re apply 1 to get the correct enum index according to the blendstate enums in engine
		int blendEnumID = _blendEnum[i] + 1; 
		_particleSettings[i].BLEND = (BLEND_STATE)blendEnumID;
	}

	//_particleEntity->RemoveComponent(_systemParticleComponent);
	//_systemParticleComponent = _particleEntity->AddComponent<ParticleSystemComponent>(); _systemParticleComponent->Init(_particleSettings, _numEmitters);
}
