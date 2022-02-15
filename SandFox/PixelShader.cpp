#include "core.h"

#include <d3dcompiler.h>

#include "PixelShader.h"

ibind::PixelShader::PixelShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob)
{
	EXC_COMINFO(D3DReadFileToBlob((c_shaderPath + shaderName + L".cso").c_str(), &pBlob));
	EXC_COMINFO(Graphics::Get().GetDevice()->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &m_pixelShader));
}

ibind::PixelShader::~PixelShader()
{
}

void ibind::PixelShader::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0u));
}
