#pragma once

#include "IBindable.h"

namespace ibind
{

	class VertexBuffer : public IBindable
	{
	public:
		template<typename T>
		VertexBuffer(const std::vector<T>& vertices, uint offset = 0);
		virtual ~VertexBuffer();

		void Bind() override;

	private:
		ComPtr<ID3D11Buffer> m_vertexBuffer;
		uint m_offset;
		uint m_stride;
	};

}

template<typename T>
ibind::VertexBuffer::VertexBuffer(const std::vector<T>& vertices, uint offset)
	:
	m_offset(offset),
	m_stride(sizeof(T))
{
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.CPUAccessFlags = 0u;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = (uint)(sizeof(T) * vertices.size());
	bDesc.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = vertices.data();

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_vertexBuffer));
}