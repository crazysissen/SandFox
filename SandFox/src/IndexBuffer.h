#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

// Previously uses short indices, 0 -> 65 536
#ifndef uindex
#define uindex unsigned int
#define UINDEX_FORMAT DXGI_FORMAT_R32_UINT
#endif

namespace SandFox
{
	namespace Bind
	{

		class FOX_API IndexBuffer : public Bindable
		{
		public:
			IndexBuffer();
			IndexBuffer(const uindex indices[], unsigned int indexCount, bool immutable = true);
			virtual ~IndexBuffer();

			void Load(const uindex indices[], unsigned int indexCount, bool immutable = true);
			void Resize(unsigned int count);
			void Update(const uindex indices[], unsigned int indexCount);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

			unsigned int GetCount() const;

		private:
			ComPtr<ID3D11Buffer> m_indexBuffer;
			unsigned int m_count;
		};

	}
}