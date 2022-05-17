#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

namespace SandFox
{
	namespace Bind
	{

		class FOX_API GeometryShader : public Bindable
		{
		public:
			GeometryShader();
			GeometryShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob);
			virtual ~GeometryShader();

			void Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob);

			void Bind(BindStage stage = BindStageNone) override;
			BindType Type() override;


		private:
			ComPtr<ID3D11GeometryShader> m_geometryShader;
		};

	}
}

