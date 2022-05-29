#include "pch.h"

#include <d3dcompiler.h>

#include "DomainShader.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::DomainShader::DomainShader()
	:
	m_domainShader(nullptr)
{
}

SandFox::Bind::DomainShader::DomainShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
	:
	m_domainShader(nullptr)
{
	Load(shaderName, pBlob);
}

SandFox::Bind::DomainShader::~DomainShader()
{
}

void SandFox::Bind::DomainShader::Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &pBlob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateDomainShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_domainShader
	));
}

void SandFox::Bind::DomainShader::Bind(BindStage stage)
{
	if (BindHandler::BindDS(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->DSSetShader(m_domainShader.Get(), nullptr, 0u));
	}
}

SandFox::BindType SandFox::Bind::DomainShader::Type()
{
	return BindTypePipeline;
}
