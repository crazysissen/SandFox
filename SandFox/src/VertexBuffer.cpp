#include "pch.h"

#include "VertexBuffer.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::VertexBuffer::VertexBuffer() 
	:
	m_vertexBuffer(nullptr),
	m_stride(0),
	m_count(0),
	m_dynamic(false)
{
}

SandFox::Bind::VertexBuffer::VertexBuffer(const void* vertices, unsigned int vertexCount, unsigned int vertexSize, bool dynamic)
	:
	m_stride(),
	m_count(),
	m_dynamic()
{
	Load(vertices, vertexCount, vertexSize, dynamic);
}

SandFox::Bind::VertexBuffer::~VertexBuffer()
{
}

void SandFox::Bind::VertexBuffer::Load(const void* vertices, unsigned int vertexCount, unsigned int vertexSize, bool dynamic)
{
	m_stride = vertexSize;
	m_count = vertexCount;
	m_dynamic = dynamic;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = dynamic * D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0u;
	bd.ByteWidth = m_stride * vertexCount;
	bd.StructureByteStride = m_stride;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = vertices;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, &srd, &m_vertexBuffer));
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

void SandFox::Bind::VertexBuffer::Bind(BindStage stage)
{
	if (BindHandler::BindVB(this))
	{
		uint offset = 0;

		EXC_COMINFO(Graphics::Get().GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &offset));
	}
}

SandFox::BindType SandFox::Bind::VertexBuffer::Type()
{
	return BindTypePipeline;
}

unsigned int SandFox::Bind::VertexBuffer::GetCount() const
{
	return m_count;
}
