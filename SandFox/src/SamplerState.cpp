#include "core.h"

#include "SamplerState.h"

SandFox::Bind::SamplerState::SamplerState(unsigned int registerIndex, D3D11_FILTER filter)
	:
	m_registerIndex(registerIndex)
{
	D3D11_SAMPLER_DESC d;

	d.Filter = filter;
	d.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	d.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
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

SandFox::Bind::SamplerState::~SamplerState()
{
}

void SandFox::Bind::SamplerState::Bind()
{
	Graphics::Get().GetContext()->PSSetSamplers(m_registerIndex, 1u, m_samplerState.GetAddressOf());
}
