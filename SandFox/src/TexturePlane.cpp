#include "pch.h"

#include "TexturePlane.h"
#include "Bindables.h"

SandFox::Prim::TexturePlane::TexturePlane(Transform t, wstring textureName, cs::Point tiling)
	:
	Drawable(t)
{
	if (StaticInit())
	{
		ComPtr<ID3DBlob> blob;

		struct Vertex
		{
			Vec3 position;
			Vec3 normal;
			Vec2 uv;
			Vec3 tangent;
		};

		Vertex vertices[] =
		{
			{ { -1, 1, 0 }, { 0, 0, -1 }, { 0, 0 }, { 1, 0, 0 } },
			{ { 1, 1, 0 }, { 0, 0, -1 }, { 1, 0 }, { 1, 0, 0 } },
			{ { 1, -1, 0 }, { 0, 0, -1 }, { 1, 1 }, { 1, 0, 0 } },
			{ { -1, -1, 0 }, { 0, 0, -1 }, { 0, 1 }, { 1, 0, 0 } }
		};

		uindex indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		BindPipeline p;
		p.vb = new SandFox::Bind::VertexBuffer(vertices, 4u, sizeof(Vertex));
		p.ib = new SandFox::Bind::IndexBuffer(indices, 6u);
		p.shader = Shader::Get(ShaderTypePhong);

		StaticSetPipeline(p);
	}

	m_materialInfoData =
	{
		{ 1.0f, 1.0f, 1.0f }, {},
		{ 1.0f, 1.0f, 1.0f }, {},
		{ 1.0f, 1.0f, 1.0f },
		64.0f,
		(Vec2)tiling
	};

	m_materialInfo = new SandFox::Bind::ConstBuffer(RegCBVMaterialInfo, &m_materialInfoData, sizeof(MaterialInfo), false);

	// Configure Drawable

	AddResource(new SandFox::Bind::STConstBuffer(*this), BindStageVS);
	AppendStage(BindStageHS);
	AppendStage(BindStageDS);

	m_texture.Load(textureName.c_str(), true);

	byte bn[] = { (byte)(127), (byte)(127), (byte)(255), 255 };
	Texture* normalTexture = new Texture();
	normalTexture->Load(bn, 1, 1, true, 4, DXGI_FORMAT_R8G8B8A8_UNORM);

	byte be[] = { (byte)(255), (byte)(255), (byte)(255), 255 };
	Texture* exponentTexture = new Texture();
	exponentTexture->Load(be, 1, 1, true, 4, DXGI_FORMAT_R8G8B8A8_UNORM);

	AddResource(m_materialInfo, BindStagePS);
	AddResource(new SandFox::TextureSRV(&m_texture, RegSRVTexColor, false), BindStagePS);
	AddResource(new SandFox::TextureSRV(exponentTexture, RegSRVTexSpecularity, true), BindStagePS);
	AddResource(new SandFox::TextureSRV(normalTexture, RegSRVTexNormal, true), BindStagePS);

	AddBind(new SandFox::Bind::STConstBuffer(*this), BindStageVS);
}

void SandFox::Prim::TexturePlane::Draw()
{
	Bind();
	ExecuteIndexed();
}

void SandFox::Prim::TexturePlane::SetUVScale(Vec2 scale)
{
	m_materialInfoData.uvScale = scale;
	m_materialInfo->Write(&m_materialInfoData);
}
