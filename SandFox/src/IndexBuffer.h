#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

// Uses short indices, 0 -> 65 536
#ifndef uindex
#define uindex unsigned int
#define uindex_format DXGI_FORMAT_R32_UINT
#endif

namespace SandFox
{
	namespace Bind
	{

		class FOX_API IndexBuffer : public IBindable
		{
		public:
			IndexBuffer(const uindex indices[], unsigned int indexCount, bool immutable = true);
			virtual ~IndexBuffer();

			void Bind() override;

			unsigned int GetCount() const;

		private:
			ComPtr<ID3D11Buffer> m_indexBuffer;
			unsigned int m_count;
		};

	}
}