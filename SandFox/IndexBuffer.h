#pragma once

#include "IBindable.h"

// Uses short indices, 0 -> 65 536
typedef ushort uindex;

namespace ibind
{
	
	class IndexBuffer : public IBindable
	{
	public:
		IndexBuffer(const std::vector<uindex>& indices, bool immutable = true);
		virtual ~IndexBuffer();

		void Bind() override;

		uint GetCount() const;	
	
	private:
		ComPtr<ID3D11Buffer> m_indexBuffer;
		uint m_count;
	};

}