#include "DebugQuadHandler.h"
#include "Entity.h";
#include "QuadComponent.h"
#include "Systems.h";
#include "SystemDefs.h";
#include "VectorHelpers.h"

DebugQuadHandler::DebugQuadHandler()
{
	_count             = 0;
	_maxPerRow         = 8;
	_width             = SystemSettings::SCREEN_WIDTH  * 0.1f;
	_height            = SystemSettings::SCREEN_HEIGHT * 0.1f;
	_spacingHorizontal = SystemSettings::SCREEN_WIDTH  * 0.11f;
	_spacingVertical   = SystemSettings::SCREEN_HEIGHT * 0.12f;
	_nextStartPosX     = SystemSettings::SCREEN_HEIGHT * 0.105f;
	_nextStartPosY     = SystemSettings::SCREEN_HEIGHT * 0.065f;
}

DebugQuadHandler::~DebugQuadHandler()
{
}

void DebugQuadHandler::AddDebugQuad(ID3D11ShaderResourceView* texture)
{
	Entity* quad = new Entity();
	quad->AddComponent<QuadComponent>()->Init(XMFLOAT2(_nextStartPosX, _nextStartPosY), XMFLOAT2(_width, _height), L"", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), true);
	quad->GetComponent<QuadComponent>()->SetTexture(texture);

	_count++;
	_nextStartPosX += _spacingHorizontal;

	if (_count == _maxPerRow)
	{
		_nextStartPosX = SystemSettings::SCREEN_HEIGHT * 0.105f;;
		_nextStartPosY += _spacingVertical;
		_count = 0;
	}

	_quadEntities.push_back(quad);
}

void DebugQuadHandler::RemoveDebugQuad(ID3D11ShaderResourceView* texture)
{
	for (int i = 0; i < _quadEntities.size(); i++)
	{
		QuadComponent* quad = _quadEntities[i]->GetComponent<QuadComponent>();
		if (quad->GetTexture() == texture)
		{
			_quadEntities[i]->RemoveEntity();
			VECTOR_HELPERS::RemoveItemFromVector(_quadEntities, _quadEntities[i]);
			break;
		}
	}
}

void DebugQuadHandler::ReplaceTexture(ID3D11ShaderResourceView* original, ID3D11ShaderResourceView* replacement)
{
	for (int i = 0; i< _quadEntities.size(); i++)
	{
		QuadComponent* quad = _quadEntities[i]->GetComponent<QuadComponent>();
		if (quad->GetTexture() == original)
		{
			quad->SetTexture(replacement);
			break;
		}
	}
}