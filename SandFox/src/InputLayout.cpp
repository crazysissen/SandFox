#include "pch.h"

#include "InputLayout.h"

// Must follow Vertex Shader!
SandFox::Bind::InputLayout::InputLayout(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& pBlob) // Must follow Vertex Shader!
{
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateInputLayout(
		elementDescriptors,
		elementCount,
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&m_inputLayout
	));
}

SandFox::Bind::InputLayout::~InputLayout()
{
}

void SandFox::Bind::InputLayout::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetInputLayout(m_inputLayout.Get()));
}
