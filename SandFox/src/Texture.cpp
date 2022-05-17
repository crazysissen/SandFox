#include "pch.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <WICTextureLoader.h>

#include "Texture.h"
#include "Graphics.h"
#include "BindHandler.h"



// Texture

SandFox::Texture::Texture()
	:
	m_texture(nullptr)
{
}

SandFox::Texture::Texture(ID3D11Texture2D* texture)
	:
	m_texture(texture)
{
}

SandFox::Texture::Texture(const Texture& copy)
	:
	m_texture(copy.m_texture)
{
}

SandFox::Texture::Texture(const std::wstring& name /*std::wstring name*/)
	:
	m_texture(nullptr)
{
	Load(name);
}

SandFox::Texture::~Texture()
{
}

void SandFox::Texture::Load(ID3D11Texture2D* texture)
{
	*m_texture.ReleaseAndGetAddressOf() = texture;
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
	td.BindFlags = bindFlags | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0 /*D3D11_CPU_ACCESS_READ*/;
	td.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA srd;
	srd.pSysMem = data;
	srd.SysMemPitch = width * stride;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateTexture2D(&td, (data == nullptr) ? nullptr : &srd, &m_texture));
}

ComPtr<ID3D11Texture2D> SandFox::Texture::GetTexture()
{
	return m_texture;
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
	Texture(texture),
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

void SandFox::RenderTexture::Load(ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv)
{
	Texture::Load(texture);
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

SandFox::TextureUAV::TextureUAV()
	:
	m_texture(nullptr),
	m_uav(nullptr),
	m_owner(false)
{
}

SandFox::TextureUAV::~TextureUAV()
{
	if (m_owner)
	{
		delete m_texture;
	}
}

SandFox::TextureUAV::TextureUAV(Texture* texture, RegUAV reg, bool ownTexture)
	:
	BindableResource(reg),
	m_texture(nullptr),
	m_uav(nullptr),
	m_owner(ownTexture)
{
	Load(texture, reg, ownTexture);
}

void SandFox::TextureUAV::Bind(BindStage stage)
{
	if (BindHandler::BindUAV(stage, this))
	{
		BindUAV(stage, GetRegUAV(), m_uav);
	}
}

SandFox::BindType SandFox::TextureUAV::Type()
{
	return BindTypeUnorderedAccess;
}

void SandFox::TextureUAV::Load(Texture* texture, RegUAV reg, bool ownTexture)
{
	m_texture = texture;
	m_owner = ownTexture;
	SetReg(reg);

	D3D11_TEXTURE2D_DESC textureDesc;
	m_texture->GetTexture()->GetDesc(&textureDesc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	desc.Format = textureDesc.Format;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	desc.Texture2D = { 0 };
	desc.Buffer = { 0, 1, 0 };

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateUnorderedAccessView(m_texture->GetTexture().Get(), &desc, &m_uav));
}

void SandFox::TextureUAV::SetOwner(bool owner)
{
	m_owner = owner;
}

void SandFox::TextureUAV::Unbind(BindStage stage)
{
	BindHandler::UnbindUAV(stage, GetRegUAV());
	UnbindUAV(stage, GetRegUAV());
}

SandFox::Texture* SandFox::TextureUAV::GetTexture()
{
	return m_texture;
}

SandFox::RenderTexture* SandFox::TextureUAV::GetRenderTexture()
{
	return (RenderTexture*)m_texture;
}

ComPtr<ID3D11UnorderedAccessView> SandFox::TextureUAV::GetUAV()
{
	return m_uav;
}

SandFox::TextureSRV::TextureSRV()
	:
	m_texture(nullptr),
	m_srv(nullptr),
	m_owner(false)
{
}

SandFox::TextureSRV::~TextureSRV()
{
	if (m_owner)
	{
		delete m_texture;
	}
}

SandFox::TextureSRV::TextureSRV(Texture* texture, RegSRV reg, bool ownTexture, int formatOverride)
	:
	BindableResource(reg),
	m_texture(nullptr),
	m_srv(nullptr),
	m_owner(ownTexture)
{
	Load(texture, reg, ownTexture, formatOverride);
}

void SandFox::TextureSRV::Bind(BindStage stage)
{
	if (BindHandler::BindSRV(stage, this))
	{
		BindSRV(stage, GetRegSRV(), m_srv);
	}
}

SandFox::BindType SandFox::TextureSRV::Type()
{
	return BindTypeShaderResource;
}

void SandFox::TextureSRV::Load(Texture* texture, RegSRV reg, bool ownTexture, int formatOverride)
{
	m_texture = texture;
	SetReg(reg);

	D3D11_TEXTURE2D_DESC desc = { 0 };
	m_texture->GetTexture()->GetDesc(&desc);
	

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = (formatOverride > -1) ? (DXGI_FORMAT)formatOverride : desc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D = { 0, 1 };

	m_srv.Detach();
	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture->GetTexture().Get(), &srvd, &m_srv));
}

void SandFox::TextureSRV::SetOwner(bool owner)
{
	m_owner = owner;
}

void SandFox::TextureSRV::Unbind(BindStage stage)
{
	BindHandler::UnbindSRV(stage, GetRegSRV());
	UnbindSRV(stage, GetRegSRV());
}

SandFox::Texture* SandFox::TextureSRV::GetTexture()
{
	return m_texture;
}

SandFox::RenderTexture* SandFox::TextureSRV::GetRenderTexture()
{
	return (RenderTexture*)m_texture;
}

ComPtr<ID3D11ShaderResourceView> SandFox::TextureSRV::GetSRV()
{
	return m_srv;
}
