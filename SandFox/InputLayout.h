#pragma once

#include "IBindable.h"

namespace ibind
{

	class InputLayout : public IBindable
	{
	public:
		// MUST BE PRECEDED BY VERTEX SHADER CREATION (pBlob must contain vertex shader data)
		InputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elementDescriptors, ComPtr<ID3DBlob>& pBlob);
		InputLayout(const InputLayout&) = delete;
		InputLayout(InputLayout&&) = delete;
		virtual ~InputLayout();

		void Bind() override;

	private:
		ComPtr<ID3D11InputLayout> m_inputLayout;
	};

}