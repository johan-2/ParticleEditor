#include "DebugStats.h"
#include "Time.h"
#include "imgui.h"
#include "SystemDefs.h"
#include "CameraManager.h"
#include "TransformComponent.h"
#include "DXManager.h"
#include "HardwareProperties.h"

DebugStats::DebugStats()
{
}

DebugStats::~DebugStats()
{
}

void DebugStats::Update()
{
	bool k = true;
	ImGuiIO& io = ImGui::GetIO();

	// get camera position
	const XMFLOAT3& camPos = CameraManager::GetInstance().GetCurrentCameraGame()->GetComponent<TransformComponent>()->GetPositionRef();

	// get hardware info
	const HardwareInfo& info = DXManager::GetInstance().GetHardwareProperties()->GetHardwareInfo();

	// set properties of next window
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.1f));
	ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH * 0.99f, SCREEN_HEIGHT * 0.01f), 0, ImVec2(1, 0));

	// create window to display information
	ImGui::Begin("test", &k, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	
	// add text properties
	ImGui::TextColored(ImVec4(1,1,1,1),"FPS : %.2f", io.Framerate);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "FrameTime : %.3f", 1000.0f / io.Framerate);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "CAMERA POS : X %.2f Y %.2f Z %.2f", camPos.x, camPos.y, camPos.z);
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "GPU : %s", info.videoCardDescription);

	// end rendering of this window
	ImGui::End();
}
