#include "pch.h"

#include "CubeInterpolated.h"
#include "Bindables.h"

SandFox::Prim::TestCube::TestCube(Vec3 position, Vec3 rotation, Vec3 size)
	:
	Drawable({ position, rotation, size })
{
	if (StaticInitialization())
	{
		ushort indices[]
		{
			0, 2, 1,	2, 3, 1,
			1, 3, 5,	3, 7, 5,
			2, 6, 3,	3, 6, 7,
			4, 5, 7,	4, 7, 6,
			0, 4, 2,	2, 4, 6,
			0, 1, 4,	1, 5, 4
		};

		struct Vertex
		{
			Vec3 pos;
			Vec3 color;
		};
		
		Vertex vertices[]
		{
			{ { -1.0f, -1.0f, -1.0f },	{ 1.0f, 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, -1.0f },	{ 0.5f, 1.0f, 1.0f } },
			{ { -1.0f, 1.0f, -1.0f },	{ 1.0f, 0.5f, 1.0f } },
			{ { 1.0f, 1.0f, -1.0f },	{ 1.0f, 1.0f, 0.5f } },
			{ { -1.0f, -1.0f, 1.0f },	{ 0.5f, 0.5f, 1.0f } },
			{ { 1.0f, -1.0f, 1.0f },	{ 0.5f, 1.0f, 0.5f } },
			{ { -1.0f, 1.0f, 1.0f },	{ 1.0f, 0.5f, 0.5f } },
			{ { 1.0f, 1.0f, 1.0f },		{ 0.5f, 0.5f, 0.5f } }
		};

		D3D11_INPUT_ELEMENT_DESC inputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ComPtr<ID3DBlob> pBlob;

		AddStaticBind(new SandFox::Bind::VertexBuffer(vertices, 8));
		AddStaticBind(new SandFox::Bind::PixelShader(Graphics::Get().ShaderPath(L"PSBasicOpaque"), pBlob));
		AddStaticBind(new SandFox::Bind::VertexShader(Graphics::Get().ShaderPath(L"VSColor"), pBlob));
		AddStaticBind(new SandFox::Bind::InputLayout(inputElements, 2, pBlob));
		AddStaticBind(new SandFox::Bind::PrimitiveTopology());
		AddStaticIndexBuffer(new SandFox::Bind::IndexBuffer(indices, 36));
	}

	AddBind(new SandFox::Bind::TConstBuffer(*this));

}
