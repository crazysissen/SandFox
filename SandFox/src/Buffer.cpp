#include "pch.h"

#include "Buffer.h"
#include "Graphics.h"



SandFox::StructuredBuffer::StructuredBuffer()
	:
	m_buffer(nullptr),
	m_bufferIndex(0)
{
}

SandFox::StructuredBuffer::StructuredBuffer(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags)
	:
	m_buffer(nullptr),
	m_bufferIndex(0)
{
	LoadBuffer(data, count, structureSize, bufferIndex, dynamic, bindFlags);
}

void SandFox::StructuredBuffer::LoadBuffer(void* data, int count, int structureSize, int bufferIndex, bool dynamic, D3D11_BIND_FLAG bindFlags)
{
	m_bufferIndex = bufferIndex;

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = bindFlags;
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : D3D11_CPU_ACCESS_READ;
	bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bd.ByteWidth = count * structureSize;
	bd.StructureByteStride = structureSize;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = data;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, (data == nullptr) ? nullptr : &srd, &m_buffer));
}

void SandFox::StructuredBuffer::Resize(int count)
{
	D3D11_BUFFER_DESC bd = {};
	m_buffer->GetDesc(&bd);

	uint originalSize = bd.ByteWidth;
	uint newSize = count * bd.StructureByteStride;
	uint copySize = originalSize < newSize ? originalSize : newSize;

	D3D11_BOX sourceBox = { 0, 0, 0, copySize, 1u, 1u };
	bd.ByteWidth = newSize;
	
	ComPtr<ID3D11Buffer> newBuffer;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, nullptr, &newBuffer));
	EXC_COMINFO(Graphics::Get().GetContext()->CopySubresourceRegion(
		newBuffer.Get(), 0u,	// Destination buffer
		0u, 0u, 0u,				// Destination coordinate
		m_buffer.Get(), 0u,		// Source buffer
		&sourceBox				// Region to copy
	));

	m_buffer = newBuffer;
}

ComPtr<ID3D11Buffer> SandFox::StructuredBuffer::GetBuffer()
{
	return m_buffer;
}



SandFox::StructuredBufferUAV::StructuredBufferUAV()
	:
	StructuredBuffer(),
	m_uav(nullptr)
{
}

SandFox::StructuredBufferUAV::StructuredBufferUAV(void* data, int count, int structureSize, int bufferIndex)
	:
	StructuredBuffer(data, count, structureSize, bufferIndex, false, D3D11_BIND_UNORDERED_ACCESS)
{
	LoadUAV(count, structureSize);
}

void SandFox::StructuredBufferUAV::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->CSSetUnorderedAccessViews(m_bufferIndex, 1u, m_uav.GetAddressOf(), nullptr));
}

void SandFox::StructuredBufferUAV::Unbind()
{
	ID3D11UnorderedAccessView* nullArray[] = { nullptr };
	EXC_COMINFO(Graphics::Get().GetContext()->CSSetUnorderedAccessViews(m_bufferIndex, 1u, nullArray, nullptr));
}

void SandFox::StructuredBufferUAV::Load(void* data, int count, int structureSize, int bufferIndex)
{
	StructuredBuffer::LoadBuffer(data, count, structureSize, bufferIndex, false, D3D11_BIND_UNORDERED_ACCESS);
	LoadUAV(count, structureSize);
}

void SandFox::StructuredBufferUAV::LoadUAV(int count, int structureSize)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer = { 0, (uint)count, 0 }; 

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateUnorderedAccessView(GetBuffer().Get(), &desc, &m_uav));

	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = 0u;
	bd.Usage = D3D11_USAGE_STAGING;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0u;
	bd.ByteWidth = count * structureSize;
	bd.StructureByteStride = structureSize;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, nullptr, &m_stageBuffer));
}

void SandFox::StructuredBufferUAV::Resize(int count)
{
	StructuredBuffer::Resize(count);


	// Unordered access view

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	m_uav->GetDesc(&desc);

	desc.Buffer = { 0, (uint)count, 0 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateUnorderedAccessView(GetBuffer().Get(), &desc, &m_uav));


	// Stage buffer

	D3D11_BUFFER_DESC bd = {};
	m_stageBuffer->GetDesc(&bd);

	uint newSize = count * bd.StructureByteStride;
	bd.ByteWidth = newSize;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateBuffer(&bd, nullptr, &m_stageBuffer));
}

void SandFox::StructuredBufferUAV::Read(void* destination, int size)
{
	EXC_COMINFO(Graphics::Get().GetContext()->CopyResource(m_stageBuffer.Get(), GetBuffer().Get()));

	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(m_stageBuffer.Get(), 0, D3D11_MAP_READ, 0, &msr));

	memcpy(destination, msr.pData, size);

	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_stageBuffer.Get(), 0));
}

void SandFox::StructuredBufferUAV::Write(void* data, int size)
{
	D3D11_MAPPED_SUBRESOURCE msr = {};
	EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(m_stageBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &msr));

	memcpy(msr.pData, data, size);

	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(m_stageBuffer.Get(), 0));
	EXC_COMINFO(Graphics::Get().GetContext()->CopyResource(GetBuffer().Get(), m_stageBuffer.Get()));
}



SandFox::StructuredBufferSRV::StructuredBufferSRV()
	:
	StructuredBuffer(),
	m_srv(nullptr)
{
}

SandFox::StructuredBufferSRV::StructuredBufferSRV(void* data, int count, int structureSize, int bufferIndex)
	:
	StructuredBuffer(data, count, structureSize, bufferIndex, true, D3D11_BIND_SHADER_RESOURCE)
{
	LoadSRV(count, structureSize);
}

void SandFox::StructuredBufferSRV::Bind()
{
	EXC_COMINFO(Graphics::Get().GetContext()->CSSetShaderResources(m_bufferIndex, 1u, m_srv.GetAddressOf()));
}

void SandFox::StructuredBufferSRV::Unbind()
{
	ID3D11ShaderResourceView* nullArray[] = { nullptr };
	EXC_COMINFO(Graphics::Get().GetContext()->CSSetShaderResources(m_bufferIndex, 1u, nullArray));
}

void SandFox::StructuredBufferSRV::Load(void* data, int count, int structureSize, int bufferIndex)
{
	StructuredBuffer::LoadBuffer(data, count, structureSize, bufferIndex, true, D3D11_BIND_SHADER_RESOURCE);
	LoadSRV(count, structureSize);
}

void SandFox::StructuredBufferSRV::LoadSRV(int count, int structureSize)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_UNKNOWN;;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

	srvd.Buffer.FirstElement = 0u;
	srvd.Buffer.ElementOffset = 0u;
	srvd.Buffer.NumElements = count;
	srvd.Buffer.ElementWidth = structureSize;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(GetBuffer().Get(), &srvd, &m_srv));
}

void SandFox::StructuredBufferSRV::Resize(int count)
{
	StructuredBuffer::Resize(count);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	m_srv->GetDesc(&srvd);

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(GetBuffer().Get(), &srvd, &m_srv));
}

void SandFox::StructuredBufferSRV::Write(void* data, int size)
{
	if (size == 0)
	{
		return;
	}

	D3D11_MAPPED_SUBRESOURCE msr = {};

	EXC_COMCHECKINFO(Graphics::Get().GetContext()->Map(GetBuffer().Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));

	memcpy(msr.pData, data, size);
	
	EXC_COMINFO(Graphics::Get().GetContext()->Unmap(GetBuffer().Get(), 0u));
}