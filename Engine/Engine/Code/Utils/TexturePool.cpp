#include "TexturePool.h"
#include "DXManager.h"
#include "DDSTextureLoader/DDSTextureLoader.h"
#include <comdef.h>
#include "Systems.h"

TexturePool::TexturePool()
{
}

TexturePool::~TexturePool()
{
	for (std::map<std::wstring, ID3D11ShaderResourceView*>::iterator it = _textures.begin(); it != _textures.end(); it++)	
		it->second->Release();
		
	_textures.clear();
}

ID3D11ShaderResourceView* TexturePool::GetTexture(const wchar_t* name)
{

	std::wstring key(name);

	std::map<std::wstring, ID3D11ShaderResourceView*>::iterator it = _textures.find(key.c_str());
	if (it != _textures.end()) 
	{
		printf("Returning existing texture : %ls\n", key.c_str());
		return it->second;
	}
	else 
	{
		ID3D11ShaderResourceView* texture;

		HRESULT result = DirectX::CreateDDSTextureFromFile(Systems::dxManager->GetDevice(), name, NULL, &texture);
		if (FAILED(result))
		{
			_com_error err(result);
			LPCTSTR msg = err.ErrorMessage();
			printf(" %s \n", msg);
			printf("failed to create texture returning nullptr\n");
			return nullptr;
		}
		else
		{
			printf("Create texture : %ls\n", key.c_str());
			_textures.insert(std::pair<std::wstring, ID3D11ShaderResourceView*>(key.c_str(), texture));
			return texture;
		}
	}	 			
}
