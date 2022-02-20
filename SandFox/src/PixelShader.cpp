#include "pch.h"

#include <d3dcompiler.h>

#include "PixelShader.h"

SandFox::Bind::PixelShader::PixelShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &pBlob));
	EXC_COMINFO(Graphics::Get().GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader));
}

SandFox::Bind::PixelShader::~PixelShader()
{
}

void SandFox::Bind::PixelShader::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u));
}
