#include "pch.h"

#include "Drawable.h"

int SandFox::_Drawable::s_typeIndexCounter = 0;
std::vector<std::vector<SandFox::IBindable*>*>* SandFox::_Drawable::s_staticBindableVectors = nullptr;

SandFox::_Drawable::_Drawable(Transform transform)
	:
	m_bindables(new std::vector<IBindable*>()),
	m_indexBuffer(nullptr),
	m_index(0),
	m_transform(transform),
	m_empty(false)
{
}

SandFox::_Drawable::~_Drawable()
{
	for (IBindable* b : *m_bindables)
	{
		delete(b);
	}

	delete(m_bindables);
}

void SandFox::_Drawable::Draw()
{
	int indexCount = BindStatic();

	if (m_empty)
	{
		return;
	}

	if (m_indexBuffer == nullptr && indexCount < 0)
	{
		DBOUT("FFS mate don't try to draw a shape without an index buffer.");
		return;
	}

	for (IBindable* b : *m_bindables)
	{
		b->Bind();
	}

	EXC_COMINFO(Graphics::Get().GetContext()->DrawIndexed(indexCount < 0 ? m_indexBuffer->GetCount() : indexCount, 0u, 0u));
}

void SandFox::_Drawable::AddBind(IBindable* bindable)
{
	m_bindables->emplace_back(bindable);
}

void SandFox::_Drawable::AddIndexBuffer(SandFox::Bind::IndexBuffer* indexBuffer)
{
	m_indexBuffer = indexBuffer;

	AddBind(indexBuffer);
}

void SandFox::_Drawable::SetShader(Shader* shader)
{
	m_shader = shader;
}

dx::XMMATRIX SandFox::_Drawable::GetTransformationMatrix()
{
	return m_transform.GetMatrix();
}

bool SandFox::_Drawable::operator>(const _Drawable& ref)
{
	return m_index > ref.m_index;
}

void SandFox::_Drawable::ReleaseStatics()
{
	if (s_staticBindableVectors == nullptr)
	{
		return;
	}

	for (std::vector<IBindable*>*& v : *s_staticBindableVectors)
	{
		for (IBindable*& b : *v)
		{
			delete b;
		}

		delete v;
	}

	delete s_staticBindableVectors;
}
