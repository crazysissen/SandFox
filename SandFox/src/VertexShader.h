#pragma once

#include "SandFoxCore.h"
#include "IBindable.h"

#include <string>

namespace SandFox
{
	namespace Bind
	{

		class FOX_API VertexShader : public IBindable
		{
		public:
			VertexShader();
			VertexShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob);
			virtual ~VertexShader();

			void Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob);

			void Bind() override;

		private:
			ComPtr<ID3D11VertexShader> m_vertexShader;
		};

	}
}