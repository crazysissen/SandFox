#include "pch.h"
#include "Cubemap.h"

#include "Graphics.h"
#include <WICTextureLoader.h>



SandFox::Cubemap::Cubemap()
{
}

SandFox::Cubemap::~Cubemap()
{
}

void SandFox::Cubemap::Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage)
{
	SetReg(reg);

	Load(reg, path, usage, D3D11_BIND_SHADER_RESOURCE);
}

void SandFox::Cubemap::Load(RegSRV reg, const std::wstring& path, D3D11_USAGE usage, D3D11_BIND_FLAG additionalBindFlags)
{
	SetReg(reg);



	// Load source image

	ComPtr<ID3D11Resource> resource;
	ComPtr<ID3D11Texture2D> sourceTexture;

	EXC_COMCHECKINFO(
		DirectX::CreateWICTextureFromFileEx(
			Graphics::Get().GetDevice().Get(),
			nullptr,
			path.c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			0,
			0,
			0,
			dx::WIC_LOADER_IGNORE_SRGB | dx::WIC_LOADER_FORCE_RGBA32 | dx::WIC_LOADER_DEFAULT,
			&resource,
			nullptr)
	);

	resource.As<ID3D11Texture2D>(&sourceTexture);



	// Verify texture

	D3D11_TEXTURE2D_DESC sourceDesc = {};
	sourceTexture->GetDesc(&sourceDesc);

	uint width = sourceDesc.Width / 4;
	uint height = sourceDesc.Height / 3;

	if (width * 4 != sourceDesc.Width || height * 3 != sourceDesc.Height)
	{
		FOX_ERROR_F("Cubemap texture with total size [%i x %i] cannot be loaded, as at least one side isn't evenly divisible. The texture should adhere to the following layout:\n| -Y-- |\n| XZXZ |\n| -Y-- |", width, height);
		return;
	}

	if (width != height)
	{
		FOX_ERROR_F("Cubemap with faces of size [%i x %i] cannot be loaded, as their width and heigh must match.", width, height);
		return;
	}



	// Create cubemap texture and SRV

	D3D11_TEXTURE2D_DESC td;
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 6;
	td.Format = sourceDesc.Format;
	td.CPUAccessFlags = 0;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateTexture2D(&td, nullptr, &m_texture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvd.TextureCube.MipLevels = td.MipLevels;
	srvd.TextureCube.MostDetailedMip = 0;

	EXC_COMCHECKINFO(Graphics::Get().GetDevice()->CreateShaderResourceView(m_texture.Get(), &srvd, &m_srv));



	// Copy the textures into their respective slots

	Point targets[6] =
	{
		{ 2, 1 }, { 0, 1 },		// X
		{ 1, 0 }, { 1, 2 },		// Y
		{ 1, 1 }, { 3, 1 }		// Z
	};

	for (int i = 0; i < 6; i++)
	{
		D3D11_BOX box = 
		{ 
			targets[i].x * width, targets[i].y* height, 0u, 
			(targets[i].x + 1) * width, (targets[i].y + 1) * height, 1u
		};

		Graphics::Get().GetContext()->CopySubresourceRegion(m_texture.Get(), i, 0u, 0u, 0u, sourceTexture.Get(), 0u, &box);
	}
}

void SandFox::Cubemap::Bind(BindStage stage)
{
	if (BindHandler::BindSRV(stage, this))
	{
		BindSRV(stage, GetRegSRV(), m_srv);
	}
}

SandFox::BindType SandFox::Cubemap::Type()
{
	return BindTypeShaderResource;
}

SandFox::CubemapDrawable::CubemapDrawable(Transform t, Cubemap* cubemap, bool ownCubemap)
	:
	Drawable(t),
	m_cubemap(cubemap),
	m_owner(ownCubemap)
{
	if (StaticInit())
	{
		Vec3 vertices[8] =
		{
			{ 1, 1, 1 },
			{ -1, 1, 1 },
			{ -1, 1, -1 },
			{ 1, 1, -1 },

			{ 1, -1, 1 },
			{ -1, -1, 1 },
			{ -1, -1, -1 },
			{ 1, -1, -1 }
		};

		uindex indices[36] =
		{
			0, 3, 7,	0, 7, 4,	// +X
			2, 1, 5,	2, 5, 6,	// -X
			0, 1, 2,	0, 2, 3,	// +Y
			4, 7, 6,	4, 6, 5,	// -Y
			1, 0, 4,	1, 4, 5,	// +Z
			3, 2, 6,	3, 6, 7		// -Z
		};

		BindPipeline p;
		p.vb = new SandFox::Bind::VertexBuffer(vertices, 8, sizeof(Vec3), false);
		p.ib = new SandFox::Bind::IndexBuffer(indices, 36);
		p.shader = Shader::Get(ShaderTypeCubemap);

		StaticSetPipeline(p);
		StaticAddSampler(RegSamplerStandard, BindStagePS);
	}



	// Configure Drawable

	AddBind(new SandFox::Bind::STConstBuffer(*this), BindStageVS);
}

SandFox::CubemapDrawable::~CubemapDrawable()
{
	if (m_owner)
	{
		delete m_cubemap;
	}
}

void SandFox::CubemapDrawable::Draw()
{
	Graphics::Get().SetDepthStencilWrite(false);

	m_cubemap->Bind(BindStagePS);
	Bind();
	ExecuteIndexed();

	Graphics::Get().SetDepthStencilWrite(true);
}
