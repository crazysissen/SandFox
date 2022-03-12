#include "pch.h"
#include "Shader.h"



SandFox::Shader* SandFox::Shader::s_presets[ShaderTypeCount];



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

void SandFox::Shader::Bind()
{
	m_ps.Bind();
	m_vs.Bind();
	m_il.Bind();
	m_pt.Bind();
}

SandFox::Shader* SandFox::Shader::Get(ShaderType preset)
{
	return s_presets[(int)preset];
}

void SandFox::Shader::LoadPresets(GraphicsTechnique technique)
{
	if (technique == GraphicsTechniqueImmediate)
	{
		ComPtr<ID3DBlob> blob;

		D3D11_INPUT_ELEMENT_DESC iePhong[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

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
		ComPtr<ID3DBlob> blob;

		D3D11_INPUT_ELEMENT_DESC iePhong[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

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
	}
}

void SandFox::Shader::UnloadPresets()
{
	for (int i = 0; i < ShaderTypeCount; i++)
	{
		delete s_presets[i];
	}
}