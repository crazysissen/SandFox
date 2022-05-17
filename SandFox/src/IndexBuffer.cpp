#include "pch.h"

#include "IndexBuffer.h"
#include "Graphics.h"
#include "BindHandler.h"

inline SandFox::Bind::IndexBuffer::IndexBuffer()
	:
	m_indexBuffer(nullptr),
	m_count(0)
{
}

inline SandFox::Bind::IndexBuffer::IndexBuffer(const uindex indices[], unsigned int indexCount, bool immutable)
	:
	m_count(indexCount)
{
	Load(indices, indexCount, immutable);
}

SandFox::Bind::IndexBuffer::~IndexBuffer()
{
}

void SandFox::Bind::IndexBuffer::Load(const uindex indices[], unsigned int indexCount, bool immutable)
{
	m_count = indexCount;

	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bDesc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	bDesc.CPUAccessFlags = immutable ? 0u : D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = indexCount * sizeof(uindex);
	bDesc.StructureByteStride = sizeof(uindex);

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = indices;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_indexBuffer));
}

void SandFox::Bind::IndexBuffer::Resize(unsigned int count)
{
	m_count = count;

	D3D11_BUFFER_DESC bd = {};
	m_indexBuffer->GetDesc(&bd);

	uint originalSize = bd.ByteWidth;
	uint newSize = count * sizeof(uindex);
	uint copySize = originalSize < newSize ? originalSize : newSize;

	D3D11_BOX sourceBox = { 0, 0, 0, copySize, 1u, 1u };
	bd.ByteWidth = newSize;

	ComPtr<ID3D11Buffer> newBuffer;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, nullptr, &newBuffer));
	EXC_COMINFO(Graphics::Get().GetContext()->CopySubresourceRegion(
		newBuffer.Get(), 0u,	// Destination buffer
		0u, 0u, 0u,				// Destination coordinate
		m_indexBuffer.Get(), 0u,		// Source buffer
		&sourceBox				// Region to copy
	));

	m_indexBuffer = newBuffer;
}

void SandFox::Bind::IndexBuffer::Update(const uindex indices[], unsigned int indexCount)
{
	if (m_count == 0)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE msr = {};

	EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(m_indexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

	memcpy(msr.pData, indices, indexCount * sizeof(uindex));

	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_indexBuffer.Get(), 0u));
}

void SandFox::Bind::IndexBuffer::Bind(BindStage stage)
{
	if (BindHandler::BindIB(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->IASetIndexBuffer(m_indexBuffer.Get(), UINDEX_FORMAT, 0u));
	}
}

SandFox::BindType SandFox::Bind::IndexBuffer::Type()
{
	return BindTypePipeline;
}

uint SandFox::Bind::IndexBuffer::GetCount() const
{
	return m_count;
}
