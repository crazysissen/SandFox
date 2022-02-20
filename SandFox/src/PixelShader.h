#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API PixelShader : public IBindable
		{
		public:
			PixelShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob);
			virtual ~PixelShader();

			void Bind() override;

		private:
			ComPtr<ID3D11PixelShader> m_pixelShader;
		};

	}
}