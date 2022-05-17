#include "pch.h"
#include "TexSRVArray.h"

#include "Graphics.h"
#include "BindHandler.h"

SandFox::TexSRVArray::TexSRVArray()
	:
	m_count(0),
	m_owner(false),
	m_textures(nullptr),
	m_srvs(nullptr)
{
}

SandFox::TexSRVArray::TexSRVArray(RegSRV reg, Texture* textures, int count, bool ownTextures, int formatOverride)
{
	Load(reg, textures, count, ownTextures, formatOverride);
}

SandFox::TexSRVArray::~TexSRVArray()
{
	for (int i = 0; i < m_count; i++)
	{
		m_srvs[i]->Release();
	}

	delete[] m_srvs;

	if (m_owner)
	{
		delete[] m_textures;
	}
}

void SandFox::TexSRVArray::Load(RegSRV reg, Texture* textures, int count, bool ownTextures, int formatOverride)
{
	if (m_count != count)
	{
		m_count = count;
		delete[] m_srvs;
		m_srvs = new ID3D11ShaderResourceView*[m_count] { nullptr };
	}

	m_textures = textures;
	m_owner = ownTextures;
	SetReg(reg);

	for (int i = 0; i < m_count; i++)
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		textures[i].GetTexture()->GetDesc(&desc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = (formatOverride > -1) ? (DXGI_FORMAT)formatOverride : desc.Format;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D = { 0, 1 };

		if (m_srvs[i] != nullptr)
		{
			m_srvs[i]->Release();
		}

		EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(textures[i].GetTexture().Get(), &srvd, &(m_srvs[i])));
	}
}

void SandFox::TexSRVArray::Bind(BindStage stage)
{
	if (BindHandler::BindSRV(stage, this))
	{
		switch (stage)
		{
		//case SandFox::BindStageVS:
		//	break;
		//case SandFox::BindStageHS:
		//	break;
		//case SandFox::BindStageDS:
		//	break;
		//case SandFox::BindStageGS:
		//	break;

		case SandFox::BindStagePS:
			Graphics::Get().GetContext()->PSSetShaderResources((uint)GetReg(), m_count, m_srvs);
			break;

		case SandFox::BindStageCS:
			Graphics::Get().GetContext()->CSSetShaderResources((uint)GetReg(), m_count, m_srvs);
			break;

		default:
			FOX_WARN_F("TexSRVArray binding to stage [%i] not supported.", stage);
			break;
		}
	}
}

SandFox::BindType SandFox::TexSRVArray::Type()
{
	return BindTypeShaderResource;
}
