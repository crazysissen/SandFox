#include "pch.h"

#include "ConstantBuffer.h"
#include "Graphics.h"
#include "BindHandler.h"

SandFox::Bind::ConstBuffer::ConstBuffer()
	:
	m_constantBuffer(nullptr)
{
}

SandFox::Bind::ConstBuffer::ConstBuffer(RegCBV reg, const void* constants, unsigned int size, bool immutable)
	:
	BindableResource(reg),
	m_size(size),
	m_constantBuffer(nullptr)
{
	Load(reg, constants, size, immutable);
}

SandFox::Bind::ConstBuffer::~ConstBuffer()
{
}



void SandFox::Bind::ConstBuffer::Load(RegCBV reg, const void* constants, unsigned int size, bool immutable)
{
	m_size = size;
	SetReg(reg);

	D3D11_BUFFER_DESC bDesc = {};
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0u;
	bDesc.ByteWidth = size;
	bDesc.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA srData = {};
	srData.pSysMem = constants;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bDesc, (constants == nullptr) ? nullptr : &srData, &m_constantBuffer));
}

void SandFox::Bind::ConstBuffer::Write(const void* constants)
{
	D3D11_MAPPED_SUBRESOURCE msr = {};

	EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(m_constantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

	memcpy(msr.pData, constants, m_size);

	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_constantBuffer.Get(), 0u));
}

void SandFox::Bind::ConstBuffer::Bind(BindStage stage)
{
	if (BindHandler::BindCBV(stage, this))
	{
		BindCBV(stage, GetRegCBV(), m_constantBuffer);
	}
}

SandFox::BindType SandFox::Bind::ConstBuffer::Type()
{
	return BindTypeConstantBuffer;
}

void SandFox::Bind::ConstBuffer::Unbind(BindStage stage)
{
	BindHandler::UnbindCBV(stage, GetRegCBV());
	UnbindCBV(stage, GetRegCBV());
}