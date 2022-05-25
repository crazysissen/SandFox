#include "pch.h"
#include "Shader.h"

#include "Graphics.h"
#include "BindHandler.h"

SandFox::Shader* SandFox::Shader::s_presets[ShaderTypeCount];

bool SandFox::Shader::s_override = false;



SandFox::Shader::Shader()
	:
	m_ps(),
	m_vs(),
	m_il(),
	m_pt()
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

void SandFox::Shader::Bind(BindStage stage)
{
	if (BindHandler::BindShader(this))
	{
		m_pt.Bind(stage);

		if (!s_override)
		{
			m_il.Bind(stage);
			
			m_vs.Bind(stage); 
			m_gs.Bind(stage);
			m_ps.Bind(stage);
		}
	}
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

void SandFox::Shader::LoadPresets(GraphicsTechnique technique)
{
	D3D11_INPUT_ELEMENT_DESC iePhong[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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



	s_presets[(int)ShaderTypeParticleBasic] = new Shader();
	s_presets[(int)ShaderTypeParticleBasic]->LoadPS(Graphics::Get().ShaderPath(L"P_PSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadGS(Graphics::Get().ShaderPath(L"P_GSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadVS(Graphics::Get().ShaderPath(L"P_VSBillboard"), blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadIL(ieParticle, 2, blob);
	s_presets[(int)ShaderTypeParticleBasic]->LoadPT(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	s_presets[(int)ShaderTypeShadow] = new Shader();
	s_presets[(int)ShaderTypeShadow]->LoadVS(Graphics::Get().ShaderPath(L"VSShadow"), blob);
	s_presets[(int)ShaderTypeShadow]->LoadIL(iePhong, 3, blob);
	s_presets[(int)ShaderTypeShadow]->LoadPT();

	s_presets[(int)ShaderTypeCubemap] = new Shader();
	s_presets[(int)ShaderTypeCubemap]->LoadPS(Graphics::Get().ShaderPath(L"PSCubemap"), blob);
	s_presets[(int)ShaderTypeCubemap]->LoadVS(Graphics::Get().ShaderPath(L"VSVertex"), blob);
	s_presets[(int)ShaderTypeCubemap]->LoadIL(ieVertex, 1, blob);
	s_presets[(int)ShaderTypeCubemap]->LoadPT();

	if (technique == GraphicsTechniqueImmediate)
	{
		s_presets[(int)ShaderTypePhong] = new Shader();
		s_presets[(int)ShaderTypePhong]->LoadPS(Graphics::Get().ShaderPath(L"PSPhongTextured"), blob);
		s_presets[(int)ShaderTypePhong]->LoadVS(Graphics::Get().ShaderPath(L"VSStandard"), blob);
		s_presets[(int)ShaderTypePhong]->LoadIL(iePhong, 3, blob);
		s_presets[(int)ShaderTypePhong]->LoadPT();

		s_presets[(int)ShaderTypePhongMapped] = new Shader();
		s_presets[(int)ShaderTypePhongMapped]->LoadPS(Graphics::Get().ShaderPath(L"PSPhongMapped"), blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadVS(Graphics::Get().ShaderPath(L"VSStandard"), blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadIL(iePhong, 3, blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadPT();
	}
	else
	{
		s_presets[(int)ShaderTypePhong] = new Shader();
		s_presets[(int)ShaderTypePhong]->LoadPS(Graphics::Get().ShaderPath(L"D_PSPhongTextured"), blob);
		s_presets[(int)ShaderTypePhong]->LoadVS(Graphics::Get().ShaderPath(L"VSStandard"), blob);
		s_presets[(int)ShaderTypePhong]->LoadIL(iePhong, 3, blob);
		s_presets[(int)ShaderTypePhong]->LoadPT();

		s_presets[(int)ShaderTypePhongMapped] = new Shader();
		s_presets[(int)ShaderTypePhongMapped]->LoadPS(Graphics::Get().ShaderPath(L"D_PSPhongMapped"), blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadVS(Graphics::Get().ShaderPath(L"VSStandard"), blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadIL(iePhong, 3, blob);
		s_presets[(int)ShaderTypePhongMapped]->LoadPT();

		//s_presets[(int)ShaderTypeParticleBasic] = new Shader();
		//s_presets[(int)ShaderTypeParticleBasic]->LoadPS(Graphics::Get().ShaderPath(L"D_P_PSBillboard"), blob);
		//s_presets[(int)ShaderTypeParticleBasic]->LoadGS(Graphics::Get().ShaderPath(L"P_GSBillboard"), blob);
		//s_presets[(int)ShaderTypeParticleBasic]->LoadVS(Graphics::Get().ShaderPath(L"P_VSBillboard"), blob);
		//s_presets[(int)ShaderTypeParticleBasic]->LoadIL(ieParticle, 2, blob);
		//s_presets[(int)ShaderTypeParticleBasic]->LoadPT(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	}
}

void SandFox::Shader::UnloadPresets()
{
	for (int i = 0; i < ShaderTypeCount; i++)
	{
		delete s_presets[i];
	}
}
