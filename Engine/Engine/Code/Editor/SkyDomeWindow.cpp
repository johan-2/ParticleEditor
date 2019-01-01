#include "SkyDomeWindow.h"
#include "MasterEditor.h"
#include "imgui.h"
#include "Systemdefs.h"

SkyDomeWindow::SkyDomeWindow(MasterEditor* masterEditor) : IEditorWindow(masterEditor)
{
}

SkyDomeWindow::~SkyDomeWindow()
{
}

void SkyDomeWindow::Render()
{
	// set properties of next window;
	ImGui::SetNextWindowBgAlpha(0.6f);
	ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.6f));
	ImGui::SetNextWindowPos(ImVec2(0, 0), 0, ImVec2(0, 0));

	// create window to display information
	ImGui::Begin("Skydome Settings", nullptr);

	if (ImGui::Button("Back"))
		GoToMain();

	// end rendering of this window
	ImGui::End();
}