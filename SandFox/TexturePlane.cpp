#include "core.h"

#include "TexturePlane.h"
#include "Bindables.h"

prim::TexturePlane::TexturePlane(Transform t, wstring textureName)
	:
	Drawable(t)
{
	if (StaticInitialization())
	{
		ComPtr<ID3DBlob> blob;

		struct Vertex
		{
			Vec3 position;
			Vec3 normal;
			Vec2 uv;
		};

		struct MaterialInfo
		{
			Vec3 ambient;
			PAD1(1);

			Vec3 diffuse;
			PAD2(1);

			Vec3 specular;
			float shininess;
		}
		materialInfo = 
		{
			{ 1.0f, 1.0f, 1.0f }, 0,
			{ 1.0f, 1.0f, 1.0f }, 0,
			{ 1, 1, 1 },
			64
		};

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		std::vector<Vertex> vertices =
		{
			{ { -1, 1, 0 }, { 0, 0, -1 }, { 0, 0 } },
			{ { 1, 1, 0 }, { 0, 0, -1 }, { 1, 0 } },
			{ { 1, -1, 0 }, { 0, 0, -1 }, { 1, 1 } },
			{ { -1, -1, 0 }, { 0, 0, -1 }, { 0, 1 } }
		};

		std::vector<uindex> indices =
		{
			0, 1, 2,
			0, 2, 3
		};

		AddStaticBind(new ibind::PixelShader(L"PSPhongTextured", blob));
		AddStaticBind(new ibind::VertexShader(L"VSStandard", blob));
		AddStaticBind(new ibind::InputLayout(inputElements, blob));
		AddStaticBind(new ibind::PrimitiveTopology());

		// Light data blablabla.

		AddStaticBind(new ibind::SamplerState(5u, D3D11_FILTER_MIN_MAG_MIP_POINT));
		AddStaticBind(new ibind::TextureBindable(textureName, 4u));

		AddStaticBind(new ibind::VertexBuffer(vertices));
		AddStaticIndexBuffer(new ibind::IndexBuffer(indices));

		AddStaticBind(new ibind::ConstBufferP<MaterialInfo>(materialInfo, 2));
	}

	AddBind(new ibind::STConstBuffer(*this));
}
