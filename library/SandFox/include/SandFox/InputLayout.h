#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API InputLayout : public IBindable
		{
		public:
			// MUST BE PRECEDED BY VERTEX SHADER CREATION (pBlob must contain vertex shader data)
			InputLayout(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& pBlob);
			InputLayout(const InputLayout&) = delete;
			InputLayout(InputLayout&&) = delete;
			virtual ~InputLayout();

			void Bind() override;

		private:
			ComPtr<ID3D11InputLayout> m_inputLayout;
		};

	}
}