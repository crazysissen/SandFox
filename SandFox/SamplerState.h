#pragma once

#include "IBindable.h"

namespace ibind
{

	class SamplerState : public IBindable
	{
	public:
		SamplerState(uint registerIndex, D3D11_FILTER filter);
		virtual ~SamplerState();
		 
		void Bind() override;

	private:
		uint m_registerIndex;
		ComPtr<ID3D11SamplerState> m_samplerState;

	};

}