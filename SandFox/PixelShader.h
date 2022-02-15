#pragma once

#include "IBindable.h"

namespace ibind
{

	class PixelShader : public IBindable
	{
	public:
		PixelShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob);
		virtual ~PixelShader();

		void Bind() override;

	private:
		ComPtr<ID3D11PixelShader> m_pixelShader;
	};

}