#include "pch.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>

#include "Texture.h"
#include "Graphics.h"



// Texture

SandFox::Texture::Texture()
	:
	m_texture(nullptr),
	m_resourceView(nullptr)
{
}

SandFox::Texture::Texture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv)
	:
	m_texture(texture),
	m_resourceView(srv)
{
}

SandFox::Texture::Texture(const Texture& copy)
	:
	m_texture(copy.m_texture),
	m_resourceView(copy.m_resourceView)
{
}

SandFox::Texture::Texture(const std::wstring& name /*std::wstring name*/)
	:
	m_texture(nullptr),
	m_resourceView(nullptr)
{
	Load(name);
}

SandFox::Texture::~Texture()
{
}

void SandFox::Texture::Load(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv)
{
	*m_texture.ReleaseAndGetAddressOf() = texture;
	*m_resourceView.ReleaseAndGetAddressOf() = srv;
}

void SandFox::Texture::Load(const std::wstring& name, bool immutable, D3D11_BIND_FLAG bindFlags)
{
	// Load texture using DXTK

	ComPtr<ID3D11Resource> resource;

	EXC_COMCHECKINFO(
		DirectX::CreateWICTextureFromFileEx(
			Graphics::Get().GetDevice().Get(), 
			Graphics::Get().GetContext().Get(), 
			name.c_str(), 
			0,
			immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DYNAMIC,
			bindFlags,
			0,
			0,
			dx::WIC_LOADER_IGNORE_SRGB | dx::WIC_LOADER_FORCE_RGBA32 | dx::WIC_LOADER_DEFAULT,
			&resource,
			nullptr)
	);

	resource->QueryInterface(IID_ID3D11Texture2D, (void**)m_texture.GetAddressOf());

	// Create shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvd, &m_resourceView));
}

void SandFox::Texture::Load(unsigned char* data, int width, int height, bool immutable, int stride, DXGI_FORMAT format, D3D11_BIND_FLAG bindFlags)
{
	// Create Texture resource
	D3D11_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = format;
	td.SampleDesc = { 1, 0 };
	td.Usage = immutable ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
	td.BindFlags = bindFlags;
	td.CPUAccessFlags = 0 /*D3D11_CPU_ACCESS_READ*/;
	td.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = data;
	srd.SysMemPitch = width * stride;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateTexture2D(&td, (data == nullptr) ? nullptr : &srd, &m_texture));

	// Create shader resource view

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	m_resourceView.Detach();
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvd, &m_resourceView));
}

ComPtr<ID3D11Texture2D> SandFox::Texture::GetTexture()
{
	return m_texture;
}

ComPtr<ID3D11ShaderResourceView> SandFox::Texture::GetResourceView()
{
	return m_resourceView;
}



// Render Texture

SandFox::RenderTexture::RenderTexture()
	:
	Texture(),
	m_renderTarget(nullptr)
{
}

SandFox::RenderTexture::RenderTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv)
	:
	Texture(texture, srv),
	m_renderTarget(rtv)
{
}

SandFox::RenderTexture::RenderTexture(const cs::ColorA& color, int width, int height, DXGI_FORMAT format, D3D11_BIND_FLAG additionalBindFlags)
	:
	RenderTexture()
{
	Load(color, width, height, format, additionalBindFlags);
}

SandFox::RenderTexture::RenderTexture(const RenderTexture& copy)
	:
	Texture(copy),
	m_renderTarget(copy.m_renderTarget)
{
}

SandFox::RenderTexture::RenderTexture(const Texture& texture)
	:
	Texture(texture),
	m_renderTarget(nullptr)
{
	D3D11_TEXTURE2D_DESC desc;
	GetTexture()->GetDesc(&desc);
	CreateRenderTarget(desc.Format);
}

SandFox::RenderTexture::~RenderTexture()
{
}

void SandFox::RenderTexture::Load(ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv, ID3D11RenderTargetView* rtv)
{
	Texture::Load(texture, srv);
	*m_renderTarget.ReleaseAndGetAddressOf() = rtv;
}

void SandFox::RenderTexture::Load(const std::wstring& name, bool immutable, D3D11_BIND_FLAG additionalBindFlags)
{
	Texture::Load(name, false, (D3D11_BIND_FLAG)(additionalBindFlags | D3D11_BIND_RENDER_TARGET));
	CreateRenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM);
}

void SandFox::RenderTexture::Load(unsigned char* data, int width, int height, bool immutable, int stride, DXGI_FORMAT format, D3D11_BIND_FLAG additionalBindFlags)
{
	Texture::Load(data, width, height, false, stride, format, (D3D11_BIND_FLAG)(additionalBindFlags | D3D11_BIND_RENDER_TARGET));
	CreateRenderTarget(format);
}

void SandFox::RenderTexture::Load(const cs::ColorA& color, int width, int height, DXGI_FORMAT format, D3D11_BIND_FLAG additionalBindFlags)
{
	Load(nullptr, width, height, false, 4, format, additionalBindFlags);
	Clear(color);
}

void SandFox::RenderTexture::Clear(const cs::ColorA& color)
{
	float channels[] = { color.r, color.g, color.b, color.a };
	EXC_COMINFO(Graphics::Get().GetContext()->ClearRenderTargetView(m_renderTarget.Get(), channels));
}

ComPtr<ID3D11RenderTargetView> SandFox::RenderTexture::GetRenderTarget()
{
	return m_renderTarget;
}

void SandFox::RenderTexture::CreateRenderTarget(DXGI_FORMAT format)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = format;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvd.Texture2D = { 0 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateRenderTargetView(GetTexture().Get(), &rtvd, &m_renderTarget));
}



// UAV

SandFox::UAVTexture::UAVTexture()
	:
	m_texture(nullptr),
	m_uav(nullptr)
{
}

SandFox::UAVTexture::UAVTexture(Texture* texture)
	:
	m_texture(nullptr),
	m_uav(nullptr)
{
	Load(texture);
}

void SandFox::UAVTexture::Load(Texture* texture)
{
	m_texture = texture;

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetTexture()->GetDesc(&textureDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	desc.Format = textureDesc.Format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D = { 0 };
	desc.Buffer = { 0, 1, 0 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateUnorderedAccessView(m_texture->GetTexture().Get(), &desc, &m_uav));
}

ComPtr<ID3D11UnorderedAccessView> SandFox::UAVTexture::GetUAV()
{
	return m_uav;
}
