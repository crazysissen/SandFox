#include "pch.h"

#include "InputLayout.h"
#include "Graphics.h"

SandFox::Bind::InputLayout::InputLayout()
	:
	m_inputLayout(nullptr)
{
}

// Must follow Vertex Shader!
SandFox::Bind::InputLayout::InputLayout(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& blob) // Must follow Vertex Shader!
	:
	m_inputLayout(nullptr)
{
	Load(elementDescriptors, elementCount, blob);
}

SandFox::Bind::InputLayout::~InputLayout()
{
}

void SandFox::Bind::InputLayout::Load(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& blob)
{
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateInputLayout(
		elementDescriptors,
		elementCount,
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&m_inputLayout
	));
}

void SandFox::Bind::InputLayout::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetInputLayout(m_inputLayout.Get()));
}
