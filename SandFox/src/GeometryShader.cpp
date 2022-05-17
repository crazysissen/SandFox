#include "pch.h"
#include <d3dcompiler.h>

#include "GeometryShader.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::GeometryShader::GeometryShader()
	:
	m_geometryShader(nullptr)
{
}

SandFox::Bind::GeometryShader::GeometryShader(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob)
	:
	m_geometryShader(nullptr)
{
	Load(shaderName, blob);
}	

SandFox::Bind::GeometryShader::~GeometryShader()
{
}

void SandFox::Bind::GeometryShader::Load(const std::wstring& shaderName, ComPtr<ID3DBlob>& blob)
{
	EXC_COMINFO(D3DReadFileToBlob((shaderName + L".cso").c_str(), &blob));
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateGeometryShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr,
		&m_geometryShader
	));
}

void SandFox::Bind::GeometryShader::Bind(BindStage stage)
{
	if (BindHandler::BindGS(this))
	{
		EXC_COMINFO(Graphics::Get().GetContext()->GSSetShader(m_geometryShader.Get(), nullptr, 0u));
	}
}

SandFox::BindType SandFox::Bind::GeometryShader::Type()
{
	return BindTypePipeline;
}