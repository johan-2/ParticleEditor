#include "MainEditorWindow.h"
#include "SystemDefs.h"
#include "imgui.h"
#include "MasterEditor.h"
#include "GUI.h"

MainEditorWindow::MainEditorWindow(MasterEditor* masterEditor) : IEditorWindow(masterEditor)
{
}

MainEditorWindow::~MainEditorWindow()
{
}

void MainEditorWindow::Render()
{
	GUI::BeginWindow("Editor", 0.6f, SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT, 0, 0, 0, 0);
	
	if (GUI::Button("SkyDome Settings"))
		_masterEditor->activeWindow = _masterEditor->skyDomeWindow;

	if (GUI::Button("Post Processing Settings"))
		_masterEditor->activeWindow = _masterEditor->postProcessWindow;

	GUI::Text(ImVec4(1, 1, 1, 1), "Press F1 to toggle GameCamera");
	GUI::Text(ImVec4(1, 1, 1, 1), "Press F2 to toggle ShadowCamera");

	// end rendering of this window
	GUI::EndWindow();
}