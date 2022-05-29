#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API DomainShader : public Bindable
		{
		public:
			DomainShader();
			DomainShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);
			virtual ~DomainShader();

			void Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& pBlob);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;

		private:
			ComPtr<ID3D11DomainShader> m_domainShader;
		};

	}
}