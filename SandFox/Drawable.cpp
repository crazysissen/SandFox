#include "core.h"

#include "Drawable.h"

int _Drawable::s_typeIndexCounter = 0;
std::vector<std::vector<IBindable*>*>* _Drawable::s_staticBindableVectors = nullptr;

_Drawable::_Drawable(Transform transform)
	:
	m_bindables(new std::vector<IBindable*>()),
	m_indexBuffer(nullptr),
	m_index(0),
	m_transform(transform),
	m_empty(false)
{
}

_Drawable::~_Drawable()
{
	for (IBindable* b : *m_bindables)
	{
		delete(b);
	}

	delete(m_bindables);
}

void _Drawable::Draw() const
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

	EXC_COMINFO( Graphics::Get().GetContext()->DrawIndexed(indexCount < 0 ? m_indexBuffer->GetCount() : indexCount, 0u, 0u) );
}

void _Drawable::AddBind(IBindable* bindable)
{
	m_bindables->emplace_back(bindable);
}

void _Drawable::AddIndexBuffer(ibind::IndexBuffer* indexBuffer)
{
	m_indexBuffer = indexBuffer;

	AddBind(indexBuffer);
}

dx::XMMATRIX _Drawable::GetTransformationMatrix()
{
	return m_transform.GetMatrix();
}

bool _Drawable::operator>(const _Drawable& ref)
{
	return m_index > ref.m_index;
}

void _Drawable::ReleaseStatics()
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
