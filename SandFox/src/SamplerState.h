#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API SamplerState : public BindableResource
		{
		public:
			SamplerState();
			SamplerState(RegSampler reg, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_CLAMP);
			virtual ~SamplerState();

			ComPtr<ID3D11SamplerState> GetSamplerState();

			void Load(RegSampler reg, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_CLAMP);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;


		private:
			ComPtr<ID3D11SamplerState> m_samplerState;

		};

	}
}