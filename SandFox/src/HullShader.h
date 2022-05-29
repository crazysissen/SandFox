#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API HullShader : public Bindable
		{
		public:
			HullShader();
			HullShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);
			virtual ~HullShader();

			void Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

		private:
			ComPtr<ID3D11HullShader> m_hullShader;
		};

	}
}