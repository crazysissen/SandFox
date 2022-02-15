#include "core.h"

#include "IndexBuffer.h"

ibind::IndexBuffer::IndexBuffer(const std::vector<uindex>& indices, bool immutable)
	:
	m_count((uint)indices.size())
{
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	bDesc.CPUAccessFlags = 0u;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = (uint)indices.size() * sizeof(uindex);
	bDesc.StructureByteStride = sizeof(uindex);

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = indices.data();

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_indexBuffer));
}

ibind::IndexBuffer::~IndexBuffer()
{
}

void ibind::IndexBuffer::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
}

uint ibind::IndexBuffer::GetCount() const
{
	return m_count;
}
