#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API InputLayout : public Bindable
		{
		public:
			// MUST BE PRECEDED BY VERTEX SHADER CREATION (pBlob must contain vertex shader data)
			InputLayout();
			InputLayout(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& blob);
			InputLayout(const InputLayout&) = delete;
			InputLayout(InputLayout&&) = delete;
			virtual ~InputLayout();

			void Load(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& blob);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

		private:
			ComPtr<ID3D11InputLayout> m_inputLayout;
		};

	}
}