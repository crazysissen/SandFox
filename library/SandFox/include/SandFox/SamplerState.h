#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API SamplerState : public IBindable
		{
		public:
			SamplerState(unsigned int registerIndex, D3D11_FILTER filter);
			virtual ~SamplerState();

			void Bind() override;

		private:
			unsigned int m_registerIndex;
			ComPtr<ID3D11SamplerState> m_samplerState;

		};

	}
}