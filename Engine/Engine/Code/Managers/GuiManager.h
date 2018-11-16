#pragma once
#include <D3D11.h>
#include "imgui.h"

class GuiManager
{
public:
	GuiManager();
	~GuiManager();

	void Update();
	void Initialize(HWND hwnd);

	ID3D11Buffer*& GetVertexBuffer() { return _vertexBuffer; }
	ID3D11Buffer*& GetIndexBuffer()  { return _indexBuffer; }

	void UpdateMouseCursor();
		
private:
	
	// creates the buffers and texture
	void CreateBuffers();
	void CreateTexture();

	// texture for displaying the UI 
	ID3D11ShaderResourceView* _texture;

	// vertex and index buffers
	ID3D11Buffer* _vertexBuffer;
	ID3D11Buffer* _indexBuffer;

	// pointer to the window handle
	HWND _hwnd;

	ImGuiMouseCursor _lastMouseCursor = ImGuiMouseCursor_COUNT;
};

