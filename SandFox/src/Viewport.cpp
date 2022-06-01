#include "pch.h"
#include "Viewport.h"

#include "Graphics.h"

SandFox::Viewport::Viewport()
	:
	m_viewport()
{
}

SandFox::Viewport::~Viewport()
{
}

void SandFox::Viewport::Load(unsigned int width, unsigned int height, Vec2 corner, float minDepth, float maxDepth)
{
	m_dimensions = { width, height }; 

	m_viewport = {};
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = minDepth;
	m_viewport.MaxDepth = maxDepth;
	m_viewport.TopLeftX = corner.x;
	m_viewport.TopLeftY = corner.y;
}

void SandFox::Viewport::Apply()
{
	EXC_COMINFO(Graphics::Get().GetContext()->RSSetViewports(1u, &m_viewport));
}

cs::UPoint SandFox::Viewport::GetDimensions()
{
	return m_dimensions;
}