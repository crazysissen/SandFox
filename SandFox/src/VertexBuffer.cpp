#include "core.h"

#include "VertexBuffer.h"

SandFox::Bind::VertexBuffer::~VertexBuffer()
{
}

void SandFox::Bind::VertexBuffer::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->IASetVertexBuffers(0u, 1u, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset));
}
