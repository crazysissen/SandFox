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
		s->transform = t;
	}
}

void SandFox::Prim::MeshDrawable::SetUVScaleAll(Vec2 scale)
{
	for (SubmeshDrawable* s : m_submeshes)
	{
		s->SetUVScale(scale);
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
	m_index(index),
	m_uvScale(1, 1)
{
	if (StaticInit())
	{
		StaticAddSampler(RegSamplerStandard, BindStagePS);
	}

	MeshSubmesh& s = m->submeshes[index];
	MeshMaterial& mt = m->materials[s.materialIndex];

	MaterialInfo mi =
	{
		mt.ambient, {},
		mt.diffuse, {},
		mt.specular,
		mt.exponent,
		m_uvScale
	}; 

	BindPipeline p;
	p.vb = new SandFox::Bind::VertexBuffer(m->vertices, m->vertexCount, sizeof(MeshVertex));
	p.ib = new SandFox::Bind::IndexBuffer(s.indices, s.indexCount);
	p.shader = Shader::Get(ShaderTypePhongMapped);

	m_materialInfo = new SandFox::Bind::ConstBuffer(RegCBVMaterialInfo, &mi, sizeof(MaterialInfo), false);



	// Configure Drawable

	SetPipeline(p);

	AddBind(new SandFox::Bind::STConstBuffer(*this), BindStageVS);
	AddResource(m_materialInfo, BindStagePS);
	AddResource(new SandFox::TextureSRV(&m->textures[mt.albedoIndex], RegSRVTexColor, false), BindStagePS);
	AddResource(new SandFox::TextureSRV(&m->textures[mt.exponentIndex], RegSRVTexSpecularity, false), BindStagePS);
}

void SandFox::Prim::MeshDrawable::SubmeshDrawable::Draw()
{
	Bind();
	ExecuteIndexed();
}

void SandFox::Prim::MeshDrawable::SubmeshDrawable::SetUVScale(Vec2 scale)
{
	m_uvScale = scale;
	
	MeshSubmesh& s = m_mesh->submeshes[m_index];
	MeshMaterial& mt = m_mesh->materials[s.materialIndex];

	MaterialInfo mi =
	{
		mt.ambient, {},
		mt.diffuse, {},
		mt.specular,
		mt.exponent,
		m_uvScale
	};

	m_materialInfo->Write(&mi);
}
