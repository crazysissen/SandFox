#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API VertexBuffer : public Bindable
		{
		public:
			VertexBuffer();
			VertexBuffer(const void* vertices, unsigned int vertexCount, unsigned int vertexSize, bool dynamic = false);
			virtual ~VertexBuffer();

			void Load(const void* vertices, unsigned int vertexCount, unsigned int vertexSize, bool dynamic = false);
			void Resize(unsigned int vertexCount);
			void Update(void* data, int vertexCount);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

			unsigned int GetCount() const;

		private:
			ComPtr<ID3D11Buffer> m_vertexBuffer;
			unsigned int m_stride;
			unsigned int m_count;
			bool m_dynamic;
		};

	}
}
