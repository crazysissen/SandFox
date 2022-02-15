#pragma once

#include "IBindable.h"

namespace ibind
{

	class VertexShader : public IBindable
	{
	public:
		VertexShader(std::wstring shaderName, ComPtr<ID3DBlob>& pBlob);
		virtual ~VertexShader();

		void Bind() override;

	private:
		ComPtr<ID3D11VertexShader> m_vertexShader;
	};

}