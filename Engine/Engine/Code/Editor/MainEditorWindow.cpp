#include "MainEditorWindow.h"
#include "SystemDefs.h"
#include "imgui.h"
#include "MasterEditor.h"

MainEditorWindow::MainEditorWindow(MasterEditor* masterEditor) : IEditorWindow(masterEditor)
{

}

MainEditorWindow::~MainEditorWindow()
{

}

void MainEditorWindow::Render()
{
	// set properties of next window
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));

	// create window to display information
	ImGui::Begin("Editor", nullptr);
	
	if (ImGui::Button("SkyDome Settings"))
		_masterEditor->activeWindow = _masterEditor->skyDomeWindow;

	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Press F1 to toggle GameCamera");
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Press F2 to toggle ShadowCamera");

	// end rendering of this window
	ImGui::End();
}