#include "pch.h"

#include "SamplerState.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::SamplerState::SamplerState()
	:
	m_samplerState(nullptr)
{
}

SandFox::Bind::SamplerState::SamplerState(RegSampler reg, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
	:
	BindableResource(reg),
	m_samplerState(nullptr)
{
	Load(reg, filter, addressMode);
}

SandFox::Bind::SamplerState::SamplerState(RegSampler reg, D3D11_FILTER filter, cs::Color borderColor)
{
	Load(reg, filter, borderColor);
}

SandFox::Bind::SamplerState::~SamplerState()
{
}

ComPtr<ID3D11SamplerState> SandFox::Bind::SamplerState::GetSamplerState()
{
	return m_samplerState;
}

void SandFox::Bind::SamplerState::Load(RegSampler reg, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	SetReg(reg);

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

void SandFox::Bind::SamplerState::Load(RegSampler reg, D3D11_FILTER filter, cs::Color borderColor)
{
	SetReg(reg);

	D3D11_SAMPLER_DESC d;

	d.Filter = filter;
	d.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	d.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	d.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP /*D3D11_TEXTURE_ADDRESS_MIRROR*/;
	d.MipLODBias = 0.0f;
	d.MaxAnisotropy = 1u/*0u*/;
	d.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	d.BorderColor[0] = borderColor.r;
	d.BorderColor[1] = borderColor.g;
	d.BorderColor[2] = borderColor.b;
	d.BorderColor[3] = 1.0f;
	d.MinLOD = 0.0f;
	d.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Get().GetDevice()->CreateSamplerState(&d, &m_samplerState);
}

void SandFox::Bind::SamplerState::Bind(BindStage stage)
{
	if (BindHandler::BindSampler(stage, this))
	{
		BindSampler(stage, GetRegSampler(), m_samplerState);
	}
}

SandFox::BindType SandFox::Bind::SamplerState::Type()
{
	return BindTypeSampler;
}
