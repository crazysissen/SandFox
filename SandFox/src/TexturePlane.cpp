#include "pch.h"

#include "TexturePlane.h"
#include "Bindables.h"

SandFox::Prim::TexturePlane::TexturePlane(Transform t, wstring textureName, cs::Point tiling)
	:
	Drawable(t)
{
	FOX_WARN("TexturePlane currently not supported.");

	if (StaticInit())
	{
		//ComPtr<ID3DBlob> blob;

		//struct Vertex
		//{
		//	Vec3 position;
		//	Vec3 normal;
		//	Vec2 uv;
		//};

		//struct MaterialInfo
		//{
		//	Vec3 ambient;
		//	PAD(1, 0);

		//	Vec3 diffuse;
		//	PAD(1, 1);

		//	Vec3 specular;
		//	float shininess;
		//}
		//materialInfo =
		//{
		//	{ 1.0f, 1.0f, 1.0f }, 0,
		//	{ 1.0f, 1.0f, 1.0f }, 0,
		//	{ 1, 1, 1 },
		//	64
		//};

		//struct TextureInfo
		//{
		//	cs::Vec2 uvScale;
		//	PAD(8, 0);
		//}
		//textureInfo =
		//{
		//	{ (float)tiling.x, (float)tiling.y }
		//};

		//D3D11_INPUT_ELEMENT_DESC inputElements[] =
		//{
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		//};

		//Vertex vertices[] =
		//{
		//	{ { -1, 1, 0 }, { 0, 0, -1 }, { 0, 0 } },
		//	{ { 1, 1, 0 }, { 0, 0, -1 }, { 1, 0 } },
		//	{ { 1, -1, 0 }, { 0, 0, -1 }, { 1, 1 } },
		//	{ { -1, -1, 0 }, { 0, 0, -1 }, { 0, 1 } }
		//};

		//uindex indices[] =
		//{
		//	0, 1, 2,
		//	0, 2, 3
		//};

		//SetStaticShader(Shader::Get(ShaderTypePhong));

		//// Light data blablabla.

		//AddStaticBind(new SandFox::Bind::SamplerState(5u, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP));
		//AddStaticBind(new SandFox::Bind::TextureBindable(textureName, 4u));

		//AddStaticBind(new SandFox::Bind::VertexBuffer(vertices, 4));
		//AddStaticIndexBuffer(new SandFox::Bind::IndexBuffer(indices, 6));

		//AddStaticBind(new SandFox::Bind::ConstBufferP<MaterialInfo>(materialInfo, 2));
		//AddStaticBind(new SandFox::Bind::ConstBufferP<TextureInfo>(textureInfo, 3));
	}

	AddBind(new SandFox::Bind::STConstBuffer(*this), BindStageVS);
}

void SandFox::Prim::TexturePlane::Draw()
{
	FOX_FTRACE("TexturePlane currently not supported, drawing disabled.");
}
