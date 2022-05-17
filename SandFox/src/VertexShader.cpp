#include "pch.h"

#include <d3dcompiler.h>

#include "VertexShader.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::VertexShader::VertexShader()
	:
	m_vertexShader(nullptr)
{
}

SandFox::Bind::VertexShader::VertexShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob)
	:
	m_vertexShader(nullptr)
{
	Load(shaderName, blob);
}

SandFox::Bind::VertexShader::~VertexShader()
{
}

void SandFox::Bind::VertexShader::Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &blob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateVertexShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_vertexShader
	));
}

void SandFox::Bind::VertexShader::Bind(BindStage stage)
{
	if (BindHandler::BindVS(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0u));
	}
}

SandFox::BindType SandFox::Bind::VertexShader::Type()
{
	return BindTypePipeline;
}
