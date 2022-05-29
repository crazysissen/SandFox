#pragma once

#include "SandFoxCore.h"
#include "GraphicsEnums.h"

#include "IBindable.h"

#include "PrimitiveTopology.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "HullShader.h"
#include "DomainShader.h"

namespace SandFox
{

	enum ShaderType
	{
		ShaderTypePhong,
		ShaderTypeParticleBasic,
		ShaderTypeShadow,
		ShaderTypeCubemap,
		ShaderTypeMirror,
		
		ShaderTypeCount // <-- Keep last
	};

	class FOX_API Shader : public Bindable
	{
	public:
		Shader(bool hasTesselation = false);

		void LoadPS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadGS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadVS(const std::wstring& path, ComPtr<ID3DBlob>& blob);

		void LoadHS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadDS(const std::wstring& path, ComPtr<ID3DBlob>& blob);
		void LoadTVS(const std::wstring& path, ComPtr<ID3DBlob>& blob); // Alternate tesselation VS
		void LoadTPT(D3D_PRIMITIVE_TOPOLOGY topology); // Alternate tesselation PT


		void LoadIL(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& pBlob);
		void LoadPT(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Bind(BindStage stage = BindStageNone) override;
		BindType Type() override;

	public:
		static Shader* Get(ShaderType preset);

		static void ShaderOverride(bool enabled);
		static void ShaderTesselation(bool enabled);
		static void ShaderTesselationCurrent(bool enabled);

		static bool GetShaderOverride();
		static bool GetShaderTesselation();

		static void LoadPresets(GraphicsTechnique technique);
		static void UnloadPresets();

	private:
		static Shader* s_presets[ShaderTypeCount];
		static bool s_override;
		static bool s_tesselation;
		static bool s_tesselationCurrent;

	private:
		bool m_hasTesselation;
		bool m_lastTesselation;

		Bind::PrimitiveTopology m_pt;
		Bind::InputLayout		m_il;

		Bind::PixelShader		m_ps;
		Bind::GeometryShader	m_gs;
		Bind::VertexShader		m_vs;

		Bind::HullShader		m_hs;
		Bind::DomainShader		m_ds;
		Bind::VertexShader		m_tvs;
		Bind::PrimitiveTopology m_tpt;
	};

}
