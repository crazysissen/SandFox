#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API PixelShader : public Bindable
		{
		public:
			PixelShader();
			PixelShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);
			virtual ~PixelShader();

			void Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

		private:
			ComPtr<ID3D11PixelShader> m_pixelShader;
		};

	}
}