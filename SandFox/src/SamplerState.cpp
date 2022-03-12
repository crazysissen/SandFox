#include "pch.h"

#include "SamplerState.h"
#include "Graphics.h"

SandFox::Bind::SamplerState::SamplerState()
	:
	m_registerIndex(0),
	m_samplerState(nullptr)
{
}

SandFox::Bind::SamplerState::SamplerState(unsigned int registerIndex, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
	:
	m_registerIndex(registerIndex),
	m_samplerState(nullptr)
{
	Load(registerIndex, filter, addressMode);
}

SandFox::Bind::SamplerState::~SamplerState()
{
}

ComPtr<ID3D11SamplerState> SandFox::Bind::SamplerState::GetSamplerState()
{
	return m_samplerState;
}

void SandFox::Bind::SamplerState::Load(unsigned int registerIndex, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	D3D11_SAMPLER_DESC d;

	d.Filter = filter;
	d.AddressU = addressMode;
	d.AddressV = addressMode;
	d.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP /*D3D11_TEXTURE_ADDRESS_MIRROR*/;
	d.MipLODBias = 0.0f;
	d.MaxAnisotropy = 1u/*0u*/;
	d.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d.BorderColor[0] = 0.0f;
	d.BorderColor[1] = 0.0f;
	d.BorderColor[2] = 0.0f;
	d.BorderColor[3] = 0.0f;
	d.MinLOD = 0.0f;
	d.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Get().GetDevice()->CreateSamplerState(&d, &m_samplerState);
}

void SandFox::Bind::SamplerState::Bind()
{
	Graphics::Get().GetContext()->PSSetSamplers(m_registerIndex, 1u, m_samplerState.GetAddressOf());
}
