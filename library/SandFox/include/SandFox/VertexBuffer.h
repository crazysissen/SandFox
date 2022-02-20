#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"
#include "Graphics.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API VertexBuffer : public IBindable
		{
		public:
			template<typename T>
			VertexBuffer(const T vertices[], unsigned int vertexCount, unsigned int offset = 0);
			virtual ~VertexBuffer();

			void Bind() override;

		private:
			ComPtr<ID3D11Buffer> m_vertexBuffer;
			unsigned int m_offset;
			unsigned int m_stride;
		};

	}
}

template<typename T>
SandFox::Bind::VertexBuffer::VertexBuffer(const T vertices[], unsigned int vertexCount, unsigned int offset)
	:
	m_offset(offset),
	m_stride(sizeof(T))
{
	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.CPUAccessFlags = 0u;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = (unsigned int)(sizeof(T) * vertexCount);
	bDesc.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = vertices;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, &srData, &m_vertexBuffer));
}