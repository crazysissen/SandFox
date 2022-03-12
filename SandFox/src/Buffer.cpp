#include "pch.h"

#include "Buffer.h"



SandFox::Bind::Buffer::Buffer()
	:
	m_buffer(nullptr)
{
}

SandFox::Bind::Buffer::Buffer(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlag)
	:
	m_buffer(nullptr)
{
}

void SandFox::Bind::Buffer::Load(void* data, int size, int bufferIndex)
{
}

void SandFox::Bind::Buffer::Resize(int size)
{
}

void SandFox::Bind::Buffer::Update(void* data, int size)
{
}

void SandFox::Bind::Buffer::Read(void* destination, int size)
{
}



SandFox::Bind::BufferUAV::BufferUAV()
	:
	Buffer(),
	m_uav(nullptr)
{
}

SandFox::Bind::BufferUAV::BufferUAV(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format)
	:
	Buffer(data, size, bufferIndex, dynamic, format, DXGI_USAGE_UNORDERED_ACCESS)
{
}

void SandFox::Bind::BufferUAV::Bind()
{
}

void SandFox::Bind::BufferUAV::LoadView()
{
}



SandFox::Bind::BufferSRV::BufferSRV()
	:
	Buffer(),
	m_srv(nullptr)
{
}

SandFox::Bind::BufferSRV::BufferSRV(void* data, int size, int bufferIndex, bool dynamic, DXGI_FORMAT format)
	:
	Buffer(data, size, bufferIndex, dynamic, format, DXGI_USAGE_)
{
}

void SandFox::Bind::BufferSRV::Bind()
{
}

void SandFox::Bind::BufferSRV::LoadView()
{
}
