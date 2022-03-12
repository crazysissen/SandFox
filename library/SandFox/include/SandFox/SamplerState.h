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
			SamplerState();
			SamplerState(unsigned int registerIndex, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_CLAMP);
			virtual ~SamplerState();

			ComPtr<ID3D11SamplerState> GetSamplerState();

			void Load(unsigned int registerIndex, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_CLAMP);

			void Bind() override;

		private:
			unsigned int m_registerIndex;
			ComPtr<ID3D11SamplerState> m_samplerState;

		};

	}
}