#include "pch.h"

#include "VertexBuffer.h"

SandFox::Bind::VertexBuffer::VertexBuffer() 
	:
	m_vertexBuffer(nullptr),
	m_stride(0),
	m_dynamic(false)
{
}

SandFox::Bind::VertexBuffer::~VertexBuffer()
{
}

void SandFox::Bind::VertexBuffer::Resize(unsigned int vertexCount)
{
	D3D11_BUFFER_DESC bd = {};
	m_vertexBuffer->GetDesc(&bd);

	uint originalSize = bd.ByteWidth;
	uint newSize = vertexCount * m_stride;
	uint copySize = originalSize < newSize ? originalSize : newSize;

	D3D11_BOX sourceBox = { 0, 0, 0, copySize, 1u, 1u };
	bd.ByteWidth = newSize;

	ComPtr<ID3D11Buffer> newBuffer;
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, nullptr, &newBuffer));

	EXC_COMINFO(Graphics::Get().GetContext()->CopySubresourceRegion(
		newBuffer.Get(), 0u,		// Destination buffer
		0u, 0u, 0u,					// Destination coordinate
		m_vertexBuffer.Get(), 0u,	// Source buffer
		&sourceBox					// Region to copy
	));

	m_vertexBuffer = newBuffer;
}

void SandFox::Bind::VertexBuffer::Update(void* data, int vertexCount)
{
	if (vertexCount == 0)
	{
		return;
	}

	if (!m_dynamic)
	{
		EXC("Cannot update non-dynamic vertex buffer.");
	}

	D3D11_MAPPED_SUBRESOURCE msr = {};

	EXC_COMINFO(Graphics::Get().GetContext()->Map(m_vertexBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

	memcpy(msr.pData, data, m_stride * vertexCount);

	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_vertexBuffer.Get(), 0u));
}

void SandFox::Bind::VertexBuffer::Bind()
{
	uint offset = 0;

	EXC_COMINFO(Graphics::Get().GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset));
}
