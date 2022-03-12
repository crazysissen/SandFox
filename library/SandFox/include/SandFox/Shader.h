#pragma once

#include "SandFoxCore.h"
#include "Graphics.h"
#include "Bindables.h"

namespace SandFox
{

	enum ShaderType
	{
		ShaderTypePhong,
		ShaderTypePhongMapped,
		
		ShaderTypeCount // <-- Keep last
	};

	class FOX_API Shader : public IBindable
	{
	public:
		Shader();

		void LoadPS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadVS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadIL(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& pBlob);
		void LoadPT(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Bind() override;

	public:
		static Shader* Get(ShaderType preset);

		static void LoadPresets(GraphicsTechnique technique);
		static void UnloadPresets();

	private:
		static Shader* s_presets[ShaderTypeCount];

	private:
		Bind::PixelShader		m_ps;
		Bind::VertexShader		m_vs;
		Bind::InputLayout		m_il;
		Bind::PrimitiveTopology m_pt;
	};

}
