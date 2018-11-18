#pragma once
#include <d3d11.h>
#include <map>

class TexturePool
{
public:
	TexturePool();
	~TexturePool();

	// get a SRV by name
	ID3D11ShaderResourceView* GetTexture(const wchar_t* name);

private:

	// map of SRV's by name
	std::map<std::wstring, ID3D11ShaderResourceView*> _textures;	
};

