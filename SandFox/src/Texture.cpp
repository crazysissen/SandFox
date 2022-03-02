#include "pch.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>

#include "Texture.h"
#include "Graphics.h"

SandFox::Texture::Texture()
	:
	m_texture(nullptr),
	m_resourceView(nullptr)
{
}
SandFox::Texture::Texture(const Texture& copy)
	:
	m_texture(copy.m_texture),
	m_resourceView(copy.m_resourceView)
{
}

SandFox::Texture::Texture(std::wstring name /*std::wstring name*/)
	:
	m_texture(nullptr),
	m_resourceView(nullptr)
{
	Load(name);
}

SandFox::Texture::~Texture()
{
}

void SandFox::Texture::Load(std::wstring name)
{
	// Load texture using DXTK

	EXC_COMCHECKINFO(
		DirectX::CreateWICTextureFromFileEx(
			Graphics::Get().GetDevice().Get(), 
			Graphics::Get().GetContext().Get(), 
			name.c_str(), 
			0,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			dx::WIC_LOADER_IGNORE_SRGB | dx::WIC_LOADER_FORCE_RGBA32 | dx::WIC_LOADER_DEFAULT,
			&m_texture,
			nullptr)
	);

	// Create shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvd, &m_resourceView));
}

void SandFox::Texture::Load(unsigned char* data, int width, int height, bool immutable, int stride, DXGI_FORMAT format)
{
	// Create Texture resource
	D3D11_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;
	td.SampleDesc = { 1, 0 };
	td.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = data;
	srd.SysMemPitch = width * stride;

	ID3D11Texture2D* texture;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateTexture2D(&td, &srd, &texture));

	texture->QueryInterface(IID_ID3D11Resource, (void**)&m_texture);

	// Create shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	m_resourceView.Detach();
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvd, &m_resourceView));
}

ComPtr<ID3D11ShaderResourceView>& SandFox::Texture::GetResourceView()
{
	return m_resourceView;
}
