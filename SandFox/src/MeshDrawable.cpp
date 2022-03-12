#include "pch.h"
#include "MeshDrawable.h"

#include "Bindables.h"

SandFox::Prim::MeshDrawable::MeshDrawable(Transform transform)
	:
	m_transform(transform),
	m_mesh()
{
}

SandFox::Prim::MeshDrawable::MeshDrawable(Transform transform, const Mesh& mesh)
	:
	m_transform(transform),
	m_mesh(mesh)
{
	for (int i = 0; i < m_mesh.submeshCount; i++)
	{
		m_submeshes.Add(new SubmeshDrawable(transform, &m_mesh, i));
	}
}

SandFox::Prim::MeshDrawable::~MeshDrawable()
{
	for (SubmeshDrawable* s : m_submeshes)
	{
		delete s;
	}
}

void SandFox::Prim::MeshDrawable::Load(const Mesh& mesh)
{
	for (int i = 0; i < m_submeshes.Size(); i++)
	{
		delete m_submeshes[i];
	}

	m_submeshes.Clear();
	m_mesh = mesh;

	for (int i = 0; i < m_mesh.submeshCount; i++)
	{
		m_submeshes.Add(new SubmeshDrawable(m_transform, &m_mesh, i));
	}
}

SandFox::Transform SandFox::Prim::MeshDrawable::GetTransform()
{
	return m_transform;
}

void SandFox::Prim::MeshDrawable::SetTransform(Transform t)
{
	m_transform = t;
	for (SubmeshDrawable* s : m_submeshes)
	{
		s->m_transform = t;
	}
}

void SandFox::Prim::MeshDrawable::Draw()
{
	for (SubmeshDrawable* s : m_submeshes)
	{
		s->Draw();
	}
}

SandFox::Prim::MeshDrawable::SubmeshDrawable::SubmeshDrawable(Transform t, Mesh* m, int index)
	:
	Drawable(t),
	m_mesh(m),
	m_index(index)
{
	if (StaticInitialization())
	{
		ComPtr<ID3DBlob> blob;

		D3D11_INPUT_ELEMENT_DESC inputElements[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		AddStaticBind(Shader::Get(ShaderTypePhongMapped));

		/*AddStaticBind(new SandFox::Bind::PixelShader(Graphics::Get().ShaderPath(L"PSPhongMapped"), blob));
		AddStaticBind(new SandFox::Bind::VertexShader(Graphics::Get().ShaderPath(L"VSStandard"), blob));
		AddStaticBind(new SandFox::Bind::InputLayout(inputElements, 3, blob));
		AddStaticBind(new SandFox::Bind::PrimitiveTopology());*/
	}

	MeshSubmesh& s = m->submeshes[index];
	MeshMaterial& mt = m->materials[s.materialIndex];

	AddBind(new SandFox::Bind::SamplerState(4u, D3D11_FILTER_MIN_MAG_MIP_POINT));
	AddBind(new SandFox::Bind::TextureBindable(m->textures[mt.ambientMapIndex], 5u));
	AddBind(new SandFox::Bind::TextureBindable(m->textures[mt.diffuseMapIndex], 6u));
	AddBind(new SandFox::Bind::TextureBindable(m->textures[mt.specularMapIndex], 7u));
	AddBind(new SandFox::Bind::TextureBindable(m->textures[mt.exponentMapIndex], 8u));

	AddBind(new SandFox::Bind::VertexBuffer(m->vertices, m->vertexCount));
	AddIndexBuffer(new SandFox::Bind::IndexBuffer(s.indices, s.indexCount));

	AddBind(new SandFox::Bind::STConstBuffer(*this));
}
