#include "pch.h"

#include <d3dcompiler.h>

#include "HullShader.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::HullShader::HullShader()
	:
	m_hullShader(nullptr)
{
}

SandFox::Bind::HullShader::HullShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
	:
	m_hullShader(nullptr)
{
	Load(shaderName, pBlob);
}

SandFox::Bind::HullShader::~HullShader()
{
}

void SandFox::Bind::HullShader::Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &pBlob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateHullShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_hullShader
	));
}

void SandFox::Bind::HullShader::Bind(BindStage stage)
{
	if (BindHandler::BindHS(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->HSSetShader(m_hullShader.Get(), nullptr, 0u));
	}
}

SandFox::BindType SandFox::Bind::HullShader::Type()
{
	return BindTypePipeline;
}
