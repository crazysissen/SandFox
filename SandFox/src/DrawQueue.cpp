#include "pch.h"
#include "DrawQueue.h"

SandFox::DrawQueue::DrawQueue()
{
}

SandFox::DrawQueue::~DrawQueue()
{
}

void SandFox::DrawQueue::Init(bool autoClear)
{
	m_autoClear = autoClear;
}

void SandFox::DrawQueue::DeInit()
{
	m_autoClear = false;
}

void SandFox::DrawQueue::AddPre(IDrawable* drawable)
{
	m_preDrawables.Add(drawable);
}

void SandFox::DrawQueue::AddMain(IDrawable* drawable)
{
	m_mainDrawables.Add(drawable);
}

void SandFox::DrawQueue::AddPost(IDrawable* drawable)
{
	m_postDrawables.Add(drawable);
}

void SandFox::DrawQueue::Clear()
{
	m_preDrawables.Clear(false);
	m_mainDrawables.Clear(false);
	m_postDrawables.Clear(false);
}

void SandFox::DrawQueue::DrawPre()
{
	for (int i = 0; i < m_preDrawables.Size(); i++)
	{
		m_preDrawables[i]->Draw();
	}

	if (m_autoClear)
	{
		Clear();
	}
}

void SandFox::DrawQueue::DrawMain()
{
	for (int i = 0; i < m_mainDrawables.Size(); i++)
	{
		m_mainDrawables[i]->Draw();
	}

	if (m_autoClear)
	{
		Clear();
	}
}

void SandFox::DrawQueue::DrawPost()
{
	for (int i = 0; i < m_postDrawables.Size(); i++)
	{
		m_postDrawables[i]->Draw();
	}

	if (m_autoClear)
	{
		Clear();
	}
}
