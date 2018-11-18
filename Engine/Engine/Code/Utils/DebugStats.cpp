#include "DebugStats.h"
#include "Time.h"
#include "imgui.h"
#include "SystemDefs.h"
#include "CameraManager.h"
#include "TransformComponent.h"
#include "DXManager.h"
#include "HardwareProperties.h"
#include "Systems.h"
#include "FreeMoveComponent.h"

DebugStats::DebugStats() :
	_showToolWindow(false)
{
}

DebugStats::~DebugStats()
{
}

void DebugStats::Update()
{
	bool k = true;
	ImGuiIO& io = ImGui::GetIO();

	CameraManager& CM = *Systems::cameraManager;
	DXManager& DXM    = *Systems::dxManager;
	Input& input      = *Systems::input;

	FreeMoveComponent* gameCameraMove   = CM.GetCurrentCameraGame()->GetComponent<FreeMoveComponent>();
	FreeMoveComponent* shadowCameraMove = CM.GetCurrentCameraDepthMap()->GetComponent<FreeMoveComponent>();

	if (input.IskeyPressed(DIK_F1))	
		gameCameraMove->SetActive(!gameCameraMove->IsActive());	

	if (input.IskeyPressed(DIK_F2))	
		shadowCameraMove->SetActive(!shadowCameraMove->IsActive());	

	// get camera position
	const XMFLOAT3& camPos = CM.GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionRef();

	// get hardware info
	const HardwareInfo& info = DXM.GetHardwareProperties()->GetHardwareInfo();

	// set properties of next window
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.09f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.99f, SCREEN_HEIGHT * 0.01f), 0, ImVec2(1, 0));

	// create window to display information
	ImGui::Begin("test", &k, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	
	// add text properties
	ImGui::TextColored(ImVec4(1,1,1,1),"FPS : %.2f", io.Framerate);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "FrameTime : %.3f", 1000.0f / io.Framerate);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "CAMERA POS : X %.2f Y %.2f Z %.2f", camPos.x, camPos.y, camPos.z);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "GPU : %s", info.videoCardDescription);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Press F9 to toggle tool window");
	
	// end rendering of this window
	ImGui::End();

	if (input.IskeyPressed(DIK_F9))
		_showToolWindow = !_showToolWindow;

	if (_showToolWindow)
	{
		// set properties of next window
		ImGui::SetNextWindowBgAlpha(0.6f);
		ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.25f));
		ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.99f, SCREEN_HEIGHT * 0.99f), 0, ImVec2(1, 1));

		// create window to display information
		ImGui::Begin("tool window", &k, ImGuiWindowFlags_NoTitleBar);

		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Press F1 to toggle GameCamera");
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "Press F2 to toggle ShadowCamera");

		// end rendering of this window
		ImGui::End();
	}
}
