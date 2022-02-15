#include "core.h"

#include "InputLayout.h"

// Must follow Vertex Shader!
ibind::InputLayout::InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elementDescriptors, ComPtr<ID3DBlob>& pBlob) // Must follow Vertex Shader!
{
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateInputLayout(
		elementDescriptors.data(),
		(uint)std::size(elementDescriptors),
		pBlob->GetBufferPointer(),
		pBlob->GetBufferSize(),
		&m_inputLayout
	));
}

ibind::InputLayout::~InputLayout()
{
}

void ibind::InputLayout::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetInputLayout(m_inputLayout.Get()));
}
