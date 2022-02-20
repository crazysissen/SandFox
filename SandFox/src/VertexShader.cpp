#include "core.h"

#include <d3dcompiler.h>

#include "VertexShader.h"

SandFox::Bind::VertexShader::VertexShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &pBlob));
	EXC_COMINFO(Graphics::Get().GetDevice()->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_vertexShader));
}

SandFox::Bind::VertexShader::~VertexShader()
{
}

void SandFox::Bind::VertexShader::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0u));
}
