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
			VertexBuffer();
			template<typename T>
			VertexBuffer(const T vertices[], unsigned int vertexCount, bool dynamic = false);
			virtual ~VertexBuffer();

			template<typename T>
			void Load(const T vertices[], unsigned int vertexCount, bool dynamic = false);
			void Resize(unsigned int vertexCount);
			void Update(void* data, int vertexCount);
			void Bind() override;

		private:
			ComPtr<ID3D11Buffer> m_vertexBuffer;
			unsigned int m_stride;
			bool m_dynamic;
		};

	}
}

template<typename T>
SandFox::Bind::VertexBuffer::VertexBuffer(const T vertices[], unsigned int vertexCount, bool dynamic)
	:
	m_stride(),
	m_dynamic()
{
	Load(vertices, vertexCount, dynamic);
}

template<typename T>
inline void SandFox::Bind::VertexBuffer::Load(const T vertices[], unsigned int vertexCount, bool dynamic)
{
	m_stride = sizeof(T);
	m_dynamic = dynamic;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = dynamic * D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0u;
	bd.ByteWidth = (unsigned int)(sizeof(T) * vertexCount);
	bd.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = vertices;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, &srd, &m_vertexBuffer));
}