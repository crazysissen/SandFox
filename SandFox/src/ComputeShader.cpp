#include "pch.h"

#include <d3dcompiler.h>

#include "ComputeShader.h"
#include "Graphics.h"

SandFox::ComputeShader::ComputeShader()
	:
	m_computeShader(nullptr)
{
}

SandFox::ComputeShader::ComputeShader(const std::wstring& shaderPath)
	:
	m_computeShader(nullptr)
{
	Load(shaderPath);
}

void SandFox::ComputeShader::Load(const std::wstring& shaderPath)
{
	ComPtr<ID3DBlob> pBlob;
	EXC_COMCHECKINFO(D3DReadFileToBlob((shaderPath + L".cso").c_str(), &pBlob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateComputeShader(
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		nullptr,
		&m_computeShader
	)); 
}

void SandFox::ComputeShader::Dispatch(unsigned int x, unsigned int y, unsigned int z)
{
	Graphics::Get().GetContext()->CSSetShader(m_computeShader.Get(), nullptr, 0u);
	Graphics::Get().GetContext()->Dispatch(x, y, z);
	Graphics::Get().GetContext()->CSSetShader(nullptr, nullptr, 0u);
}
