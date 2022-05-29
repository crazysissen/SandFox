#include "pch.h"
#include "Shader.h"

#include "Graphics.h"
#include "BindHandler.h"

SandFox::Shader* SandFox::Shader::s_presets[ShaderTypeCount];

bool SandFox::Shader::s_override = false;
bool SandFox::Shader::s_tesselation = false;
bool SandFox::Shader::s_tesselationCurrent = false;



SandFox::Shader::Shader(bool hasTesselation)
	:
	m_ps(),
	m_gs(),
	m_vs(),
	m_hs(),
	m_ds(),
	m_tvs(),
	m_il(),
	m_pt(),

	m_hasTesselation(hasTesselation),
	m_lastTesselation(false)
{
}

void SandFox::Shader::LoadPS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_ps.Load(path, blob);
}

void SandFox::Shader::LoadVS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_vs.Load(path, blob);
}

void SandFox::Shader::LoadIL(const D3D11_INPUT_ELEMENT_DESC elementDescriptors[], unsigned int elementCount, ComPtr<ID3DBlob>& blob)
{
	m_il.Load(elementDescriptors, elementCount, blob);
}

void SandFox::Shader::LoadPT(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_pt.Load(topology);
}

void SandFox::Shader::LoadGS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_gs.Load(path, blob);
}

void SandFox::Shader::LoadHS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_hs.Load(path, blob);
}

void SandFox::Shader::LoadDS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_ds.Load(path, blob);
}

void SandFox::Shader::LoadTVS(const std::wstring& path, ComPtr<ID3DBlob>& blob)
{
	m_tvs.Load(path, blob);
}

void SandFox::Shader::LoadTPT(D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_tpt.Load(topology);
}

void SandFox::Shader::Bind(BindStage stage)
{
	if (BindHandler::BindShader(this) || (m_hasTesselation && s_tesselationCurrent != m_lastTesselation))
	{
		if (!s_override)
		{
			m_il.Bind(stage);

			m_gs.Bind(stage);
			m_ps.Bind(stage);

			if (m_hasTesselation && s_tesselation && s_tesselationCurrent)
			{
				m_tpt.Bind(stage);
				m_tvs.Bind(stage);
				m_hs.Bind(stage);
				m_ds.Bind(stage);
			}
			else
			{
				m_pt.Bind(stage);
				m_vs.Bind(stage);

				BindHandler::UnbindHS();
				BindHandler::UnbindDS();
				Graphics::Get().GetContext()->HSSetShader(nullptr, nullptr, 0);
				Graphics::Get().GetContext()->DSSetShader(nullptr, nullptr, 0);
			}
		}
	}

	m_lastTesselation = s_tesselationCurrent;
}

SandFox::BindType SandFox::Shader::Type()
{
	return BindTypePipeline;
}

SandFox::Shader* SandFox::Shader::Get(ShaderType preset)
{
	return s_presets[(int)preset];
}

void SandFox::Shader::ShaderOverride(bool enabled)
{
	s_override = enabled;
}

void SandFox::Shader::ShaderTesselation(bool enabled)
{
	s_tesselation = enabled;
}

void SandFox::Shader::ShaderTesselationCurrent(bool enabled)
{
	s_tesselationCurrent = enabled;
}

bool SandFox::Shader::GetShaderOverride()
{
	return s_override;
}

bool SandFox::Shader::GetShaderTesselation()
{
	return s_tesselation;
}

void SandFox::Shader::LoadPresets(GraphicsTechnique technique)
{
	D3D11_INPUT_ELEMENT_DESC iePhong[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC ieParticle[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	D3D11_INPUT_ELEMENT_DESC ieVertex[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	ComPtr<ID3DBlob> blob;



	// Same between forward and deferred

	s_presets[(int)ShaderTypeParticleBasic] = new Shader();
	s_presets[(int)ShaderTypeParticleBasic]->LoadPS(Graphics::Get().ShaderPath(L"P_PSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadGS(Graphics::Get().ShaderPath(L"P_GSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadVS(Graphics::Get().ShaderPath(L"P_VSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadIL(ieParticle, 2, blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadPT(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	s_presets[(int)ShaderTypeShadow] = new Shader();
	s_presets[(int)ShaderTypeShadow]->LoadVS(Graphics::Get().ShaderPath(L"VSShadow"), blob);
	s_presets[(int)ShaderTypeShadow]->LoadIL(iePhong, 4, blob);
	s_presets[(int)ShaderTypeShadow]->LoadPT();

	s_presets[(int)ShaderTypeCubemap] = new Shader();
	s_presets[(int)ShaderTypeCubemap]->LoadPS(Graphics::Get().ShaderPath(L"PSCubemap"), blob);
	s_presets[(int)ShaderTypeCubemap]->LoadVS(Graphics::Get().ShaderPath(L"VSVertex"), blob);
	s_presets[(int)ShaderTypeCubemap]->LoadIL(ieVertex, 1, blob);
	s_presets[(int)ShaderTypeCubemap]->LoadPT();

	s_presets[(int)ShaderTypeMirror] = new Shader(true);
	s_presets[(int)ShaderTypeMirror]->LoadPS(Graphics::Get().ShaderPath(L"PSMirror"), blob);
	s_presets[(int)ShaderTypeMirror]->LoadDS(Graphics::Get().ShaderPath(L"DSPhongMirror"), blob);
	s_presets[(int)ShaderTypeMirror]->LoadHS(Graphics::Get().ShaderPath(L"HSDistance"), blob);
	s_presets[(int)ShaderTypeMirror]->LoadTVS(Graphics::Get().ShaderPath(L"VSPass"), blob);
	s_presets[(int)ShaderTypeMirror]->LoadVS(Graphics::Get().ShaderPath(L"VSMirror"), blob);
	s_presets[(int)ShaderTypeMirror]->LoadIL(iePhong, 4, blob);
	s_presets[(int)ShaderTypeMirror]->LoadPT();
	s_presets[(int)ShaderTypeMirror]->LoadTPT(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);



	// Different between forward and deferred

	s_presets[(int)ShaderTypePhong] = new Shader(true);
	s_presets[(int)ShaderTypePhong]->LoadDS(Graphics::Get().ShaderPath(L"DSPhong"), blob);
	s_presets[(int)ShaderTypePhong]->LoadHS(Graphics::Get().ShaderPath(L"HSDistance"), blob);
	s_presets[(int)ShaderTypePhong]->LoadTVS(Graphics::Get().ShaderPath(L"VSPass"), blob);
	s_presets[(int)ShaderTypePhong]->LoadVS(Graphics::Get().ShaderPath(L"VSStandard"), blob);
	s_presets[(int)ShaderTypePhong]->LoadIL(iePhong, 4, blob);
	s_presets[(int)ShaderTypePhong]->LoadPT();
	s_presets[(int)ShaderTypePhong]->LoadTPT(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	if (technique == GraphicsTechniqueImmediate)
	{
		s_presets[(int)ShaderTypePhong]->LoadPS(Graphics::Get().ShaderPath(L"PSPhong"), blob);
	}
	else
	{
		s_presets[(int)ShaderTypePhong]->LoadPS(Graphics::Get().ShaderPath(L"D_PSPhong"), blob);
	}
}

void SandFox::Shader::UnloadPresets()
{
	for (int i = 0; i < ShaderTypeCount; i++)
	{
		delete s_presets[i];
	}
}
