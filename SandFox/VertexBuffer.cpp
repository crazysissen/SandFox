#include "core.h"

#include "VertexBuffer.h"

ibind::VertexBuffer::~VertexBuffer()
{
}

void ibind::VertexBuffer::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset));
}
