#pragma once

#include "SandFoxCore.h"

namespace SandFox
{

	class FOX_API ComputeShader
	{
	public:
		ComputeShader();
		ComputeShader(const std::wstring& shaderPath);

		void Load(const std::wstring& shaderPath);

		void Dispatch(unsigned int x, unsigned int y = 1, unsigned int z = 1);

	private:
		ComPtr<ID3D11ComputeShader> m_computeShader;
	};

}