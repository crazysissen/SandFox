#include "pch.h"

#include <d3dcompiler.h>

#include "PixelShader.h"
#include "Graphics.h"

SandFox::Bind::PixelShader::PixelShader()
	:
	m_pixelShader(nullptr)
{
}

SandFox::Bind::PixelShader::PixelShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
	:
	m_pixelShader(nullptr)
{
	Load(shaderName, pBlob);
}

SandFox::Bind::PixelShader::~PixelShader()
{
}

void SandFox::Bind::PixelShader::Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &pBlob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreatePixelShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_pixelShader
	));
}

void SandFox::Bind::PixelShader::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u));
}
