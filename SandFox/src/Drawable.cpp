#include "pch.h"

#include "Drawable.h"
#include "Graphics.h"



cs::List<SandFox::IBindable*> SandFox::DrawableBase::s_allStatics;
SandFox::DrawableID SandFox::DrawableBase::s_idCounter(1);



//int SandFox::_Drawable::s_typeIndexCounter = 0;
//std::vector<std::vector<SandFox::Bindable*>*>* SandFox::_Drawable::s_staticBindableVectors = nullptr;
//
//SandFox::_Drawable::_Drawable(Transform transform)
//	:
//	m_bindables(new std::vector<Bindable*>()),
//	m_indexBuffer(nullptr),
//	m_index(0),
//	m_transform(transform)
//{
//}
//
//SandFox::_Drawable::~_Drawable()
//{
//	for (Bindable* b : *m_bindables)
//	{
//		delete(b);
//	}
//
//	delete(m_bindables);
//}
//
//void SandFox::_Drawable::Draw()
//{
//	int indexCount = BindStatic();
//
//	SandFox::Shader* shader = GetShader();
//	shader->Bind();
//
//	if (m_indexBuffer == nullptr && indexCount < 0)
//	{
//		DBOUT("Tried to draw indexed drawable without index buffer.");
//		return;
//	}
//
//	for (Bindable* b : *m_bindables)
//	{
//		b->Bind();
//	}
//
//
//	EXC_COMINFO(Graphics::Get().GetContext()->DrawIndexed(indexCount < 0 ? m_indexBuffer->GetCount() : indexCount, 0u, 0u));
//}
//
//void SandFox::_Drawable::AddBind(Bindable* bindable)
//{
//	m_bindables->emplace_back(bindable);
//}
//
//void SandFox::_Drawable::AddIndexBuffer(SandFox::Bind::IndexBuffer* indexBuffer)
//{
//	m_indexBuffer = indexBuffer;
//
//	AddBind(indexBuffer);
//}
//
//void SandFox::_Drawable::SetShader(Shader* shader)
//{
//	m_shader = shader;
//}
//
//dx::XMMATRIX SandFox::_Drawable::GetTransformationMatrix()
//{
//	return m_transform.GetMatrix();
//}
//
//bool SandFox::_Drawable::operator>(const _Drawable& ref)
//{
//	return m_index > ref.m_index;
//}
//
//void SandFox::_Drawable::ReleaseStatics()
//{
//	if (s_staticBindableVectors == nullptr)
//	{
//		return;
//	}
//
//	for (std::vector<Bindable*>*& v : *s_staticBindableVectors)
//	{
//		for (Bindable*& b : *v)
//		{
//			delete b;
//		}
//
//		delete v;
//	}
//
//	delete s_staticBindableVectors;
//}

void SandFox::DrawableBase::Execute()
{
	Graphics::Get().GetContext()->Draw(m_configuredPipeline->vb->GetCount(), 0u);
}

void SandFox::DrawableBase::ExecuteIndexed()
{
	if (!m_configuredPipeline->ib)
	{
		FOX_WARN("Cannot execute indexed draw without index buffer.");
		return;
	}

	Graphics::Get().GetContext()->DrawIndexed(m_configuredPipeline->ib->GetCount(), 0u, 0u);
}

SandFox::DrawableBase::DrawableBase(Transform transform)
	:
	transform(transform),
	m_configuredPipeline(nullptr),
	m_id(s_idCounter++)
{
}

SandFox::DrawableBase::~DrawableBase()
{
	for (IBindable* b : m_bindables)
	{
		delete b;
	}

	if (m_configuredPipeline == &m_pipeline)
	{
		delete m_pipeline.vb;
		delete m_pipeline.ib;
	}
}

void SandFox::DrawableBase::ReleaseStatics()
{
	for (IBindable* b : s_allStatics)
	{
		delete b;
	}
}

void SandFox::DrawableBase::AddToAllStatics(IBindable* bindable)
{
	s_allStatics.Add(bindable);
}
