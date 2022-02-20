#include "pch.h"

#include "IndexBuffer.h"

SandFox::Bind::IndexBuffer::IndexBuffer(const uindex indices[], unsigned int indexCount, bool immutable)
	:
	m_count(indexCount)
{
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	bDesc.CPUAccessFlags = 0u;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = indexCount * sizeof(uindex);
	bDesc.StructureByteStride = sizeof(uindex);

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = indices;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_indexBuffer));
}

SandFox::Bind::IndexBuffer::~IndexBuffer()
{
}

void SandFox::Bind::IndexBuffer::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
}

uint SandFox::Bind::IndexBuffer::GetCount() const
{
	return m_count;
}
