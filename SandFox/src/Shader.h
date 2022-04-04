#pragma once

#include "SandFoxCore.h"
#include "GraphicsEnums.h"

#include "PrimitiveTopology.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"

namespace SandFox
{

	enum ShaderType
	{
		ShaderTypePhong,
		ShaderTypePhongMapped,
		ShaderTypeParticleBasic,
		
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
		void LoadGS(const std::wstring& path, ComPtr<ID3DBlob>& blob);

		void Bind() override;

	public:
		static Shader* Get(ShaderType preset);

		static void LoadPresets(GraphicsTechnique technique);
		static void UnloadPresets();

	private:
		static Shader* s_presets[ShaderTypeCount];

	private:
		Bind::PrimitiveTopology m_pt;
		Bind::InputLayout		m_il;
		Bind::VertexShader		m_vs;
		Bind::GeometryShader	m_gs;
		Bind::PixelShader		m_ps;

	};

}
